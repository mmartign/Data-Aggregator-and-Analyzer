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
#include "config.hpp"

#include "logging.hpp"

#include <fstream>
#include <sstream>
#include <vector>

std::string ensureTrailingSlash(const std::string& url)
{
    if (url.empty() || url.back() == '/') return url;
    return url + "/";
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
