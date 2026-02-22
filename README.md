# OR-Edge Medical Data Fusion & Analysis (C++)

**Part of the Spazio IT OR-Edge Project**

A high-performance C++ application that fuses **audio transcription**
and **video semantic data** using Large Language Models, producing:

-   📊 A structured intermediate **CSV fusion table**
-   🧠 A final **medical quality & standards analysis report**

Designed for clinical, surgical, and operating-room edge environments.

------------------------------------------------------------------------

## 🚀 Overview

This application implements a **two-stage AI pipeline**:

### Stage 1 --- Aggregation

Combines: - Audio transcription (speech-to-text output) - Video semantic
analysis (frame-level AI detections)

Using a configurable LLM model, it generates a **clean CSV fusion
table**.

### Stage 2 --- Analysis

Uses a second LLM model to: - Analyze the fusion CSV - Produce a
structured **medical quality & compliance report**

The application supports any **OpenAI-compatible API endpoint**,
including: - OpenAI official API - OpenWebUI backends - Self-hosted LLM
gateways - Enterprise AI deployments

------------------------------------------------------------------------

## 🏗 Architecture

    Audio Transcription  ─┐
                          ├──► Aggregator Model ───► Fusion CSV
    Video Semantics     ──┘

    Fusion CSV ───────────────► Analyzer Model ───► Final Report

------------------------------------------------------------------------

## ✨ Features

-   Fully configurable via `config.ini`
-   OpenAI-compatible API support (custom `base_url`)
-   Robust JSON response parsing
-   Automatic CSV code-fence stripping
-   Debug JSON dump on API failure
-   Timestamped structured logging
-   Separate models for aggregation and analysis
-   GPLv2+ licensed

------------------------------------------------------------------------

## 📦 Requirements

-   C++17 or later
-   `nlohmann/json`
-   An OpenAI-compatible C++ client providing `openai.hpp`
-   CMake (recommended)
-   Access to an OpenAI-compatible API endpoint

------------------------------------------------------------------------

## 🔧 Build Example (CMake)

``` cmake
cmake_minimum_required(VERSION 3.16)
project(or_edge)

set(CMAKE_CXX_STANDARD 17)

find_package(nlohmann_json REQUIRED)

add_executable(or_edge main.cpp)

target_link_libraries(or_edge
    nlohmann_json::nlohmann_json
)
```

Build:

``` bash
mkdir build
cd build
cmake ..
make
```

------------------------------------------------------------------------

## ⚙ Configuration

The application reads configuration from `config.ini`.

### Example `config.ini`

``` ini
[openai]
base_url = https://api.openai.com/v1/
api_key = sk-xxxxxxxxxxxxxxxxxxxxxxxx

[aggregation]
aggregator_model = gpt-4o
analyzer_model = gpt-4o-mini

aggregator_prompt = You are performing multimodal medical data fusion...
analyzer_prompt = Analyze the following fusion table for procedural compliance...

intermediate_csv = fusion_output.csv
```

------------------------------------------------------------------------

## ▶ Usage

    ./or_edge <audio_transcription_file> <video_semantic_file> [config.ini]

Example:

    ./or_edge audio.txt video.txt config.ini

If `config.ini` is omitted, it defaults to:

    config.ini

------------------------------------------------------------------------

## 📊 Output

### Intermediate CSV

Saved to the path defined in:

`aggregation.intermediate_csv`

### Final Analysis Report

Printed to `stdout`.

------------------------------------------------------------------------

## 🧪 Debugging

If the API returns malformed or empty responses, the application
automatically writes:

-   `debug_aggregator_response.json`
-   `debug_analyzer_response.json`

These contain the raw API JSON responses for troubleshooting.

------------------------------------------------------------------------

## 📜 License

GNU General Public License v2 or later (GPLv2+)

This program is free software: you can redistribute it and/or modify\
it under the terms of the GNU General Public License as published by\
the Free Software Foundation.

See: https://www.gnu.org/licenses/

------------------------------------------------------------------------

## 🏢 About Spazio IT

Spazio - IT Soluzioni Informatiche s.a.s.\
via Manzoni 40\
46051 San Giorgio Bigarello\
Italy

https://spazioit.com

Part of the **OR-Edge Project** --- AI-powered solutions for medical
edge environments.

------------------------------------------------------------------------

## ⚠ Disclaimer

This software is provided **without warranty**.\
It is intended for research, validation, and controlled medical IT
environments.\
It does not replace certified medical decision systems.
