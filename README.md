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
-   MIT licensed

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
base_url = http://localhost:8080/api
api_key = sk-xxxxxxxxxxxxxxxxxxxxxxxx

[aggregation]
aggregator_model = nemotron-3-nano:latest
analyzer_model = medgemma:27b

intermediate_csv = fusion_table_intermediate.csv

aggregator_prompt = The two attached files are transcriptions of a medical procedure. The first one is a transcription of the audio. The second is a sequence of semantic descriptions of frames extracted from the video. Could you please fuse together the audio transcription and video descriptions sources, in a sort of table. Please use timestamps, they are available both in the audio and video. For each row please list the audio timestamtp, the audio transcription and the correpondent videos transcritpions. 
analyzer_prompt = Could you please: 1. Analyze in detail the following "fusion table", that puts together the audio transcription of a medical procedure with semantic descriptions of frames extracted from a video of the very same procedure. 2. Make a report about the medical procedure. 3. Tell me if everything was done according to standards, guidelines, suitable protocols. 4. Have you got any additional observation?

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

MIT License

See: [LICENSE](LICENSE)

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
