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
#pragma once

#include <map>
#include <string>
#include <nlohmann/json.hpp>

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

std::string ensureTrailingSlash(const std::string& url);
std::string nowTag();
void logInfo(const std::string& msg);
void logError(const std::string& msg);

std::map<std::string, std::string> parseIni(const std::string& filename);
bool loadConfig(const std::string& path, AppConfig& cfg);

std::string readTextFile(const std::string& path);
void writeTextFile(const std::string& path, const std::string& content);

std::string trimCopy(const std::string& s);
std::string stripCsvFence(const std::string& text);

void appendTextDeep(const json& node, std::string& out);
std::string extractMessageText(const json& response);
std::string extractApiError(const json& response);

std::string debugJsonPath(const std::string& tag);
