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
#include "logging.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

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
