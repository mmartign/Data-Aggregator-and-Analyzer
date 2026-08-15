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
#include "text_utils.hpp"

#include <cctype>

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
