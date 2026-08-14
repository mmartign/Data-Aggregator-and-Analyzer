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
#include <fstream>

#include "core.hpp"

namespace {

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

} // namespace

TEST_CASE("ensureTrailingSlash", "[core]")
{
    CHECK(ensureTrailingSlash("http://localhost:8080/api") == "http://localhost:8080/api/");
    CHECK(ensureTrailingSlash("http://localhost:8080/api/") == "http://localhost:8080/api/");
    CHECK(ensureTrailingSlash("") == "");
}

TEST_CASE("trimCopy strips leading and trailing whitespace", "[core]")
{
    CHECK(trimCopy("  hello  ") == "hello");
    CHECK(trimCopy("\t\nhello world\r\n") == "hello world");
    CHECK(trimCopy("no-trim") == "no-trim");
    CHECK(trimCopy("   ") == "");
    CHECK(trimCopy("") == "");
}

TEST_CASE("stripCsvFence removes a ```csv fenced block", "[core]")
{
    const std::string text = "Sure, here is the CSV:\n```csv\na,b,c\n1,2,3\n```\nLet me know if you need more.";
    CHECK(stripCsvFence(text) == "a,b,c\n1,2,3\n");
}

TEST_CASE("stripCsvFence removes a generic ``` fenced block", "[core]")
{
    const std::string text = "```\na,b\n1,2\n```";
    CHECK(stripCsvFence(text) == "a,b\n1,2\n");
}

TEST_CASE("stripCsvFence returns input unchanged when there is no fence", "[core]")
{
    const std::string text = "a,b,c\n1,2,3\n";
    CHECK(stripCsvFence(text) == text);
}

TEST_CASE("stripCsvFence returns input unchanged when the fence is not closed", "[core]")
{
    const std::string text = "```csv\na,b,c\n1,2,3\n";
    CHECK(stripCsvFence(text) == text);
}

TEST_CASE("parseIni parses sections, keys and strips comments/whitespace", "[core]")
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

TEST_CASE("parseIni returns an empty map for a missing file", "[core]")
{
    const auto config = parseIni("this_file_does_not_exist.ini");
    CHECK(config.empty());
}

TEST_CASE("loadConfig succeeds when all required keys are present", "[core]")
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

TEST_CASE("loadConfig fails when required keys are missing", "[core]")
{
    const TempFile file("test_loadConfig_missing.ini",
        "[openai]\n"
        "base_url = http://localhost:8080/api\n");

    AppConfig cfg;
    CHECK_FALSE(loadConfig(file.path(), cfg));
}

TEST_CASE("loadConfig fails when the config file cannot be read", "[core]")
{
    AppConfig cfg;
    CHECK_FALSE(loadConfig("this_file_does_not_exist.ini", cfg));
}

TEST_CASE("readTextFile/writeTextFile round-trip content", "[core]")
{
    const std::string path = "test_readwrite.txt";
    writeTextFile(path, "hello\nworld\n");
    CHECK(readTextFile(path) == "hello\nworld\n");
    std::remove(path.c_str());
}

TEST_CASE("readTextFile throws for a missing file", "[core]")
{
    CHECK_THROWS_AS(readTextFile("this_file_does_not_exist.txt"), std::runtime_error);
}

TEST_CASE("extractMessageText reads chat-completions style responses", "[core]")
{
    const json response = {
        {"choices", json::array({
            {{"message", {{"role", "assistant"}, {"content", "  hello there  "}}}}
        })}
    };
    CHECK(extractMessageText(response) == "hello there");
}

TEST_CASE("extractMessageText falls back to choices[0].text", "[core]")
{
    const json response = {
        {"choices", json::array({
            {{"text", "legacy completion text"}}
        })}
    };
    CHECK(extractMessageText(response) == "legacy completion text");
}

TEST_CASE("extractMessageText falls back to a deep text search for wrapper styles", "[core]")
{
    // No "choices" array, so extractMessageText falls back to appendTextDeep,
    // which only recurses through the recognized text-bearing keys
    // (text/content/output_text/value/response).
    const json response = {
        {"response", json::array({
            {{"type", "message"}, {"content", json::array({
                {{"type", "output_text"}, {"text", "responses api text"}}
            })}}
        })}
    };
    CHECK(extractMessageText(response) == "responses api text");
}

TEST_CASE("extractMessageText returns empty string when there is no text content", "[core]")
{
    const json response = {{"choices", json::array()}};
    CHECK(extractMessageText(response).empty());
}

TEST_CASE("extractApiError reads a string error field", "[core]")
{
    const json response = {{"error", "bad request"}};
    CHECK(extractApiError(response) == "bad request");
}

TEST_CASE("extractApiError dumps an object error field", "[core]")
{
    const json response = {{"error", {{"message", "bad request"}, {"code", 400}}}};
    CHECK(extractApiError(response) == json({{"message", "bad request"}, {"code", 400}}).dump());
}

TEST_CASE("extractApiError falls back to detail then message", "[core]")
{
    CHECK(extractApiError(json{{"detail", "not found"}}) == "not found");
    CHECK(extractApiError(json{{"message", "oops"}}) == "oops");
}

TEST_CASE("extractApiError returns empty string when there is no error", "[core]")
{
    CHECK(extractApiError(json{{"choices", json::array()}}).empty());
    CHECK(extractApiError(json::array()).empty());
}

TEST_CASE("debugJsonPath builds the expected filename", "[core]")
{
    CHECK(debugJsonPath("aggregator") == "debug_aggregator_response.json");
    CHECK(debugJsonPath("analyzer") == "debug_analyzer_response.json");
}
