// -*- coding: utf-8 -*-
//
// This file is part of the Spazio IT OR-Edge project.
//
// Copyright (C) 2026 Spazio IT
// Spazio - IT Soluzioni Informatiche s.a.s.
// via Manzoni 40
// 46051 San Giorgio Bigarello
// https://spazioit.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see https://www.gnu.org/licenses/.
//
#include <chrono>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <openai.hpp>

using json = nlohmann::json;

struct AppConfig {
    std::string baseUrl;
    std::string apiKey;
    std::string aggregatorModel;
    std::string analyzerModel;
    std::string aggregatorPrompt;
    std::string analyzerPrompt;
    std::string intermediateCsvPath;
};

static std::string ensureTrailingSlash(const std::string& url)
{
    if (url.empty() || url.back() == '/') return url;
    return url + "/";
}

static std::string nowTag()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    const std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << '[' << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << ']';
    return oss.str();
}

static void logInfo(const std::string& msg)
{
    std::cout << nowTag() << " [INFO] " << msg << '\n' << std::flush;
}

static void logError(const std::string& msg)
{
    std::cerr << nowTag() << " [ERROR] " << msg << '\n';
}

static std::map<std::string, std::string> parseIni(const std::string& filename)
{
    std::ifstream file(filename);
    std::map<std::string, std::string> config;
    if (!file.is_open()) return config;

    std::string line;
    std::string section;

    while (std::getline(file, line)) {
        const size_t commentPos = line.find_first_of(";#");
        if (commentPos != std::string::npos) line = line.substr(0, commentPos);

        const auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line.erase(0, start);

        const auto end = line.find_last_not_of(" \t\r\n");
        if (end != std::string::npos) line.erase(end + 1);
        if (line.empty()) continue;

        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        const size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);

        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        if (!section.empty()) key = section + "." + key;
        config[key] = value;
    }

    return config;
}

static bool loadConfig(const std::string& path, AppConfig& cfg)
{
    const auto config = parseIni(path);
    if (config.empty()) {
        logError("Unable to read config file: " + path);
        return false;
    }

    auto require = [&](const std::string& key, std::string& dest, std::vector<std::string>& missing) {
        const auto it = config.find(key);
        if (it == config.end() || it->second.empty()) {
            missing.push_back(key);
            return;
        }
        dest = it->second;
    };

    std::vector<std::string> missing;
    require("openai.base_url", cfg.baseUrl, missing);
    require("openai.api_key", cfg.apiKey, missing);
    require("aggregation.aggregator_model", cfg.aggregatorModel, missing);
    require("aggregation.analyzer_model", cfg.analyzerModel, missing);
    require("aggregation.aggregator_prompt", cfg.aggregatorPrompt, missing);
    require("aggregation.analyzer_prompt", cfg.analyzerPrompt, missing);
    require("aggregation.intermediate_csv", cfg.intermediateCsvPath, missing);

    if (!missing.empty()) {
        std::ostringstream oss;
        oss << "Missing required config values:";
        for (const auto& key : missing) oss << ' ' << key;
        logError(oss.str());
        return false;
    }

    cfg.baseUrl = ensureTrailingSlash(cfg.baseUrl);
    return true;
}

static std::string readTextFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Cannot open file: " + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static void writeTextFile(const std::string& path, const std::string& content)
{
    std::ofstream out(path);
    if (!out.is_open()) throw std::runtime_error("Cannot write file: " + path);
    out << content;
    if (!out) throw std::runtime_error("Failed writing file: " + path);
}

static std::string trimCopy(const std::string& s)
{
    size_t b = 0;
    while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    size_t e = s.size();
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

static std::string stripCsvFence(const std::string& text)
{
    const std::string openCsv = "```csv";
    const std::string openAny = "```";
    const std::string close = "```";

    const size_t csvStart = text.find(openCsv);
    size_t start = std::string::npos;
    if (csvStart != std::string::npos) {
        start = csvStart + openCsv.size();
    } else {
        const size_t anyStart = text.find(openAny);
        if (anyStart != std::string::npos) start = anyStart + openAny.size();
    }

    if (start == std::string::npos) return text;
    if (start < text.size() && text[start] == '\n') ++start;
    const size_t end = text.find(close, start);
    if (end == std::string::npos || end <= start) return text;
    return text.substr(start, end - start);
}

static void appendTextDeep(const json& node, std::string& out)
{
    if (node.is_string()) {
        out += node.get<std::string>();
        return;
    }

    if (node.is_array()) {
        for (const auto& item : node) appendTextDeep(item, out);
        return;
    }

    if (!node.is_object()) return;

    // Frequent text-bearing keys across OpenAI/OpenWebUI compatible backends.
    for (const char* key : {"text", "content", "output_text", "value", "response"}) {
        const auto it = node.find(key);
        if (it != node.end()) appendTextDeep(*it, out);
    }
}

static std::string extractMessageText(const json& response)
{
    // Standard chat-completions style: choices[0].message.content or choices[0].text
    const auto choicesIt = response.find("choices");
    if (choicesIt != response.end() && choicesIt->is_array() && !choicesIt->empty()) {
        const auto& first = (*choicesIt)[0];
        std::string out;
        const auto messageIt = first.find("message");
        if (messageIt != first.end() && messageIt->is_object()) {
            const auto contentIt = messageIt->find("content");
            if (contentIt != messageIt->end()) appendTextDeep(*contentIt, out);
        }

        if (out.empty()) {
            const auto textIt = first.find("text");
            if (textIt != first.end()) appendTextDeep(*textIt, out);
        }

        out = trimCopy(out);
        if (!out.empty()) return out;
    }

    // Responses API / other wrapper style fallback.
    std::string fallback;
    appendTextDeep(response, fallback);
    fallback = trimCopy(fallback);
    return fallback;
}

static std::string extractApiError(const json& response)
{
    if (!response.is_object()) return {};

    const auto errIt = response.find("error");
    if (errIt != response.end()) {
        if (errIt->is_string()) return errIt->get<std::string>();
        return errIt->dump();
    }

    const auto detailIt = response.find("detail");
    if (detailIt != response.end()) {
        if (detailIt->is_string()) return detailIt->get<std::string>();
        return detailIt->dump();
    }

    const auto msgIt = response.find("message");
    if (msgIt != response.end()) {
        if (msgIt->is_string()) return msgIt->get<std::string>();
        return msgIt->dump();
    }

    return {};
}

static std::string debugJsonPath(const std::string& tag)
{
    return "debug_" + tag + "_response.json";
}

static void printUsage(const char* argv0)
{
    std::cerr
        << "Usage: " << argv0 << " <audio_transcription_file> <video_semantic_file> [config.ini]\n"
        << "Reads config values from [openai] and [aggregation] sections.\n";
}

int main(int argc, char** argv)
{
    std::cout << std::unitbuf;

    if (argc < 3 || argc > 4) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string audioPath = argv[1];
    const std::string videoPath = argv[2];
    const std::string configPath = (argc == 4) ? argv[3] : "config.ini";

    AppConfig cfg;
    if (!loadConfig(configPath, cfg)) return 1;

    try {
        logInfo("Initializing OpenAI client");
        openai::start(cfg.apiKey, "", true, cfg.baseUrl);

        logInfo("Reading input files");
        const std::string audioText = readTextFile(audioPath);
        const std::string videoText = readTextFile(videoPath);

        logInfo("Running aggregation model: " + cfg.aggregatorModel);
        const std::string aggregationUserPrompt =
            cfg.aggregatorPrompt +
            "\n\nPlease output ONLY a valid CSV table with a header row."
            "\n\n[AUDIO_TRANSCRIPTION]\n" + audioText +
            "\n\n[VIDEO_FRAME_SEMANTICS]\n" + videoText;

        json aggregationBody = {
            {"model", cfg.aggregatorModel},
            {"messages", json::array({
                {{"role", "system"}, {"content", "You are a precise medical data fusion assistant. Reply with CSV only."}},
                {{"role", "user"}, {"content", aggregationUserPrompt}}
            })},
            {"stream", false}
        };

        const json aggregationResp = openai::chat().create(aggregationBody);
        const std::string aggregationApiErr = extractApiError(aggregationResp);
        if (!aggregationApiErr.empty()) {
            throw std::runtime_error("Aggregator API error: " + aggregationApiErr);
        }
        std::string intermediateCsv = extractMessageText(aggregationResp);
        if (intermediateCsv.empty()) {
            const std::string dbg = debugJsonPath("aggregator");
            writeTextFile(dbg, aggregationResp.dump(2));
            logError("Raw aggregator response saved to " + dbg);
            throw std::runtime_error("Aggregator returned no text content");
        }
        intermediateCsv = stripCsvFence(intermediateCsv);

        logInfo("Writing intermediate CSV: " + cfg.intermediateCsvPath);
        writeTextFile(cfg.intermediateCsvPath, intermediateCsv);

        logInfo("Running analyzer model: " + cfg.analyzerModel);
        const std::string analyzerUserPrompt =
            cfg.analyzerPrompt +
            "\n\n[FUSION_TABLE_CSV]\n" + intermediateCsv;

        json analyzerBody = {
            {"model", cfg.analyzerModel},
            {"messages", json::array({
                {{"role", "system"}, {"content", "You are a medical procedure quality and standards analyst."}},
                {{"role", "user"}, {"content", analyzerUserPrompt}}
            })},
            {"stream", false}
        };

        const json analyzerResp = openai::chat().create(analyzerBody);
        const std::string analyzerApiErr = extractApiError(analyzerResp);
        if (!analyzerApiErr.empty()) {
            throw std::runtime_error("Analyzer API error: " + analyzerApiErr);
        }
        const std::string analysis = extractMessageText(analyzerResp);
        if (analysis.empty()) {
            const std::string dbg = debugJsonPath("analyzer");
            writeTextFile(dbg, analyzerResp.dump(2));
            logError("Raw analyzer response saved to " + dbg);
            throw std::runtime_error("Analyzer returned no text content");
        }

        logInfo("Analysis completed. Printing final report to stdout");
        std::cout << analysis << '\n' << std::flush;
        return 0;
    } catch (const std::exception& e) {
        logError(e.what());
        return 1;
    }
}
