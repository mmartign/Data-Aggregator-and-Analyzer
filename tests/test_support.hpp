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

#include <cstdio>
#include <fstream>
#include <string>
#include <utility>

namespace test_support {

// Writes content to a temp file that is removed when the guard goes out of scope.
class TempFile {
public:
    TempFile(std::string path, const std::string& content) : path_(std::move(path))
    {
        std::ofstream out(path_);
        out << content;
    }
    ~TempFile() { std::remove(path_.c_str()); }
    const std::string& path() const { return path_; }

private:
    std::string path_;
};

} // namespace test_support
