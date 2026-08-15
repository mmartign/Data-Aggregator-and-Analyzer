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

#include "text_utils.hpp"

TEST_CASE("trimCopy strips leading and trailing whitespace", "[text_utils]")
{
    CHECK(trimCopy("  hello  ") == "hello");
    CHECK(trimCopy("\t\nhello world\r\n") == "hello world");
    CHECK(trimCopy("no-trim") == "no-trim");
    CHECK(trimCopy("   ") == "");
    CHECK(trimCopy("") == "");
}

TEST_CASE("stripCsvFence removes a ```csv fenced block", "[text_utils]")
{
    const std::string text = "Sure, here is the CSV:\n```csv\na,b,c\n1,2,3\n```\nLet me know if you need more.";
    CHECK(stripCsvFence(text) == "a,b,c\n1,2,3\n");
}

TEST_CASE("stripCsvFence removes a generic ``` fenced block", "[text_utils]")
{
    const std::string text = "```\na,b\n1,2\n```";
    CHECK(stripCsvFence(text) == "a,b\n1,2\n");
}

TEST_CASE("stripCsvFence returns input unchanged when there is no fence", "[text_utils]")
{
    const std::string text = "a,b,c\n1,2,3\n";
    CHECK(stripCsvFence(text) == text);
}

TEST_CASE("stripCsvFence returns input unchanged when the fence is not closed", "[text_utils]")
{
    const std::string text = "```csv\na,b,c\n1,2,3\n";
    CHECK(stripCsvFence(text) == text);
}
