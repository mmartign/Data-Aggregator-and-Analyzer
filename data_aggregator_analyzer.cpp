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
#include <iostream>
#include <string>
#include <openai.hpp>

#include "core.hpp"

static void printUsage(const char* argv0)
{
    std::cerr
        << "Usage: " << argv0 << " <audio_transcription_file> <video_semantic_file> [config.ini]\n"
        << "Reads config values from [openai] and [aggregation] sections.\n";
}

int main(int argc, char** argv)
{
    std::cout << std::unitbuf;

    if (argc < 3 || argc > 4) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string audioPath = argv[1];
    const std::string videoPath = argv[2];
    const std::string configPath = (argc == 4) ? argv[3] : "config.ini";

    AppConfig cfg;
    if (!loadConfig(configPath, cfg)) return 1;

    try {
        logInfo("Initializing OpenAI client");
        openai::start(cfg.apiKey, "", true, cfg.baseUrl);

        logInfo("Reading input files");
        const std::string audioText = readTextFile(audioPath);
        const std::string videoText = readTextFile(videoPath);

        logInfo("Running aggregation model: " + cfg.aggregatorModel);
        const std::string aggregationUserPrompt =
            cfg.aggregatorPrompt +
            "\n\nPlease output ONLY a valid CSV table with a header row."
            "\n\n[AUDIO_TRANSCRIPTION]\n" + audioText +
            "\n\n[VIDEO_FRAME_SEMANTICS]\n" + videoText;

        json aggregationBody = {
            {"model", cfg.aggregatorModel},
            {"messages", json::array({
                {{"role", "system"}, {"content", "You are a precise medical data fusion assistant. Reply with CSV only."}},
                {{"role", "user"}, {"content", aggregationUserPrompt}}
            })},
            {"stream", false}
        };

        const json aggregationResp = openai::chat().create(aggregationBody);
        const std::string aggregationApiErr = extractApiError(aggregationResp);
        if (!aggregationApiErr.empty()) {
            throw std::runtime_error("Aggregator API error: " + aggregationApiErr);
        }
        std::string intermediateCsv = extractMessageText(aggregationResp);
        if (intermediateCsv.empty()) {
            const std::string dbg = debugJsonPath("aggregator");
            writeTextFile(dbg, aggregationResp.dump(2));
            logError("Raw aggregator response saved to " + dbg);
            throw std::runtime_error("Aggregator returned no text content");
        }
        intermediateCsv = stripCsvFence(intermediateCsv);

        logInfo("Writing intermediate CSV: " + cfg.intermediateCsvPath);
        writeTextFile(cfg.intermediateCsvPath, intermediateCsv);

        logInfo("Running analyzer model: " + cfg.analyzerModel);
        const std::string analyzerUserPrompt =
            cfg.analyzerPrompt +
            "\n\n[FUSION_TABLE_CSV]\n" + intermediateCsv;

        json analyzerBody = {
            {"model", cfg.analyzerModel},
            {"messages", json::array({
                {{"role", "system"}, {"content", "You are a medical procedure quality and standards analyst."}},
                {{"role", "user"}, {"content", analyzerUserPrompt}}
            })},
            {"stream", false}
        };

        const json analyzerResp = openai::chat().create(analyzerBody);
        const std::string analyzerApiErr = extractApiError(analyzerResp);
        if (!analyzerApiErr.empty()) {
            throw std::runtime_error("Analyzer API error: " + analyzerApiErr);
        }
        const std::string analysis = extractMessageText(analyzerResp);
        if (analysis.empty()) {
            const std::string dbg = debugJsonPath("analyzer");
            writeTextFile(dbg, analyzerResp.dump(2));
            logError("Raw analyzer response saved to " + dbg);
            throw std::runtime_error("Analyzer returned no text content");
        }

        logInfo("Analysis completed. Printing final report to stdout");
        std::cout << analysis << '\n' << std::flush;
        return 0;
    } catch (const std::exception& e) {
        logError(e.what());
        return 1;
    }
}
