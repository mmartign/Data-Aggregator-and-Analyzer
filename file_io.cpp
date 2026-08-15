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
#include "file_io.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

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
