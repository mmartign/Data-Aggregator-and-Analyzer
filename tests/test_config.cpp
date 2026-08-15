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

#include "config.hpp"
#include "test_support.hpp"

using test_support::TempFile;

TEST_CASE("ensureTrailingSlash", "[config]")
{
    CHECK(ensureTrailingSlash("http://localhost:8080/api") == "http://localhost:8080/api/");
    CHECK(ensureTrailingSlash("http://localhost:8080/api/") == "http://localhost:8080/api/");
    CHECK(ensureTrailingSlash("") == "");
}

TEST_CASE("parseIni parses sections, keys and strips comments/whitespace", "[config]")
{
    const TempFile file("test_parseIni.ini",
        "; leading comment\n"
        "[openai]\n"
        "base_url = http://localhost:8080/api  ; inline comment\n"
        "api_key=sk-test\n"
        "\n"
        "[aggregation]\n"
        "aggregator_model = model-a\n");

    const auto config = parseIni(file.path());
    CHECK(config.at("openai.base_url") == "http://localhost:8080/api");
    CHECK(config.at("openai.api_key") == "sk-test");
    CHECK(config.at("aggregation.aggregator_model") == "model-a");
}

TEST_CASE("parseIni returns an empty map for a missing file", "[config]")
{
    const auto config = parseIni("this_file_does_not_exist.ini");
    CHECK(config.empty());
}

TEST_CASE("loadConfig succeeds when all required keys are present", "[config]")
{
    const TempFile file("test_loadConfig_ok.ini",
        "[openai]\n"
        "base_url = http://localhost:8080/api\n"
        "api_key = sk-test\n"
        "[aggregation]\n"
        "aggregator_model = agg-model\n"
        "analyzer_model = ana-model\n"
        "intermediate_csv = fusion.csv\n"
        "aggregator_prompt = combine please\n"
        "analyzer_prompt = analyze please\n");

    AppConfig cfg;
    REQUIRE(loadConfig(file.path(), cfg));
    CHECK(cfg.baseUrl == "http://localhost:8080/api/");
    CHECK(cfg.apiKey == "sk-test");
    CHECK(cfg.aggregatorModel == "agg-model");
    CHECK(cfg.analyzerModel == "ana-model");
    CHECK(cfg.intermediateCsvPath == "fusion.csv");
}

TEST_CASE("loadConfig fails when required keys are missing", "[config]")
{
    const TempFile file("test_loadConfig_missing.ini",
        "[openai]\n"
        "base_url = http://localhost:8080/api\n");

    AppConfig cfg;
    CHECK_FALSE(loadConfig(file.path(), cfg));
}

TEST_CASE("loadConfig fails when the config file cannot be read", "[config]")
{
    AppConfig cfg;
    CHECK_FALSE(loadConfig("this_file_does_not_exist.ini", cfg));
}
