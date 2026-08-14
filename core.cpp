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
// SPDX-License-Identifier: AGPL-3.0-only
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, version 3 only.
//
// Full license text: https://www.gnu.org/licenses/agpl-3.0.html
//
#include "core.hpp"

#include <cctype>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

std::string ensureTrailingSlash(const std::string& url)
{
    if (url.empty() || url.back() == '/') return url;
    return url + "/";
}

std::string nowTag()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    const std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << '[' << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << ']';
    return oss.str();
}

void logInfo(const std::string& msg)
{
    std::cout << nowTag() << " [INFO] " << msg << '\n' << std::flush;
}

void logError(const std::string& msg)
{
    std::cerr << nowTag() << " [ERROR] " << msg << '\n';
}

std::map<std::string, std::string> parseIni(const std::string& filename)
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

bool loadConfig(const std::string& path, AppConfig& cfg)
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

std::string readTextFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Cannot open file: " + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void writeTextFile(const std::string& path, const std::string& content)
{
    std::ofstream out(path);
    if (!out.is_open()) throw std::runtime_error("Cannot write file: " + path);
    out << content;
    if (!out) throw std::runtime_error("Failed writing file: " + path);
}

std::string trimCopy(const std::string& s)
{
    size_t b = 0;
    while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    size_t e = s.size();
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

std::string stripCsvFence(const std::string& text)
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

void appendTextDeep(const json& node, std::string& out)
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

std::string extractMessageText(const json& response)
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

std::string extractApiError(const json& response)
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

std::string debugJsonPath(const std::string& tag)
{
    return "debug_" + tag + "_response.json";
}
