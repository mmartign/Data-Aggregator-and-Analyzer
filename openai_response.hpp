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

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Parses and reports on chat-completion style responses from OpenAI/OpenWebUI
// compatible backends.
void appendTextDeep(const json& node, std::string& out);
std::string extractMessageText(const json& response);
std::string extractApiError(const json& response);

std::string debugJsonPath(const std::string& tag);
