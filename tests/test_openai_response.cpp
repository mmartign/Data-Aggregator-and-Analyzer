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

#include "openai_response.hpp"

TEST_CASE("extractMessageText reads chat-completions style responses", "[openai_response]")
{
    const json response = {
        {"choices", json::array({
            {{"message", {{"role", "assistant"}, {"content", "  hello there  "}}}}
        })}
    };
    CHECK(extractMessageText(response) == "hello there");
}

TEST_CASE("extractMessageText falls back to choices[0].text", "[openai_response]")
{
    const json response = {
        {"choices", json::array({
            {{"text", "legacy completion text"}}
        })}
    };
    CHECK(extractMessageText(response) == "legacy completion text");
}

TEST_CASE("extractMessageText falls back to a deep text search for wrapper styles", "[openai_response]")
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

TEST_CASE("extractMessageText returns empty string when there is no text content", "[openai_response]")
{
    const json response = {{"choices", json::array()}};
    CHECK(extractMessageText(response).empty());
}

TEST_CASE("extractApiError reads a string error field", "[openai_response]")
{
    const json response = {{"error", "bad request"}};
    CHECK(extractApiError(response) == "bad request");
}

TEST_CASE("extractApiError dumps an object error field", "[openai_response]")
{
    const json response = {{"error", {{"message", "bad request"}, {"code", 400}}}};
    CHECK(extractApiError(response) == json({{"message", "bad request"}, {"code", 400}}).dump());
}

TEST_CASE("extractApiError falls back to detail then message", "[openai_response]")
{
    CHECK(extractApiError(json{{"detail", "not found"}}) == "not found");
    CHECK(extractApiError(json{{"message", "oops"}}) == "oops");
}

TEST_CASE("extractApiError returns empty string when there is no error", "[openai_response]")
{
    CHECK(extractApiError(json{{"choices", json::array()}}).empty());
    CHECK(extractApiError(json::array()).empty());
}

TEST_CASE("debugJsonPath builds the expected filename", "[openai_response]")
{
    CHECK(debugJsonPath("aggregator") == "debug_aggregator_response.json");
    CHECK(debugJsonPath("analyzer") == "debug_analyzer_response.json");
}
