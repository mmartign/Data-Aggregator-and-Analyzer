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
#include "openai_response.hpp"

#include "text_utils.hpp"

void appendTextDeep(const json& node, std::string& out)
{
    if (node.is_string()) {
        out += node.get<std::string>();
        return;
    }

    if (node.is_array()) {
        for (const auto& item : node) appendTextDeep(item, out);
        return;
    }

    if (!node.is_object()) return;

    // Frequent text-bearing keys across OpenAI/OpenWebUI compatible backends.
    for (const char* key : {"text", "content", "output_text", "value", "response"}) {
        const auto it = node.find(key);
        if (it != node.end()) appendTextDeep(*it, out);
    }
}

std::string extractMessageText(const json& response)
{
    // Standard chat-completions style: choices[0].message.content or choices[0].text
    const auto choicesIt = response.find("choices");
    if (choicesIt != response.end() && choicesIt->is_array() && !choicesIt->empty()) {
        const auto& first = (*choicesIt)[0];
        std::string out;
        const auto messageIt = first.find("message");
        if (messageIt != first.end() && messageIt->is_object()) {
            const auto contentIt = messageIt->find("content");
            if (contentIt != messageIt->end()) appendTextDeep(*contentIt, out);
        }

        if (out.empty()) {
            const auto textIt = first.find("text");
            if (textIt != first.end()) appendTextDeep(*textIt, out);
        }

        out = trimCopy(out);
        if (!out.empty()) return out;
    }

    // Responses API / other wrapper style fallback.
    std::string fallback;
    appendTextDeep(response, fallback);
    fallback = trimCopy(fallback);
    return fallback;
}

std::string extractApiError(const json& response)
{
    if (!response.is_object()) return {};

    const auto errIt = response.find("error");
    if (errIt != response.end()) {
        if (errIt->is_string()) return errIt->get<std::string>();
        return errIt->dump();
    }

    const auto detailIt = response.find("detail");
    if (detailIt != response.end()) {
        if (detailIt->is_string()) return detailIt->get<std::string>();
        return detailIt->dump();
    }

    const auto msgIt = response.find("message");
    if (msgIt != response.end()) {
        if (msgIt->is_string()) return msgIt->get<std::string>();
        return msgIt->dump();
    }

    return {};
}

std::string debugJsonPath(const std::string& tag)
{
    return "debug_" + tag + "_response.json";
}
