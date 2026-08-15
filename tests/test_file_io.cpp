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
#include <catch2/catch_test_macros.hpp>
#include <cstdio>

#include "file_io.hpp"

TEST_CASE("readTextFile/writeTextFile round-trip content", "[file_io]")
{
    const std::string path = "test_readwrite.txt";
    writeTextFile(path, "hello\nworld\n");
    CHECK(readTextFile(path) == "hello\nworld\n");
    std::remove(path.c_str());
}

TEST_CASE("readTextFile throws for a missing file", "[file_io]")
{
    CHECK_THROWS_AS(readTextFile("this_file_does_not_exist.txt"), std::runtime_error);
}
