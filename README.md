# OR-Edge Medical Data Fusion & Analysis (C++)

[![CI](https://github.com/mmartign/Data-Aggregator-and-Analyzer/actions/workflows/ci.yml/badge.svg)](https://github.com/mmartign/Data-Aggregator-and-Analyzer/actions/workflows/ci.yml)
[![License: AGPL v3](https://img.shields.io/badge/License-AGPLv3-blue.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)

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
-   AGPL-3.0 licensed

------------------------------------------------------------------------

## 📦 Requirements

-   C++20 compiler
-   CMake 3.14+
-   `libcurl` development headers (e.g. `libcurl4-openssl-dev` on
    Debian/Ubuntu, `curl` via Homebrew on macOS)
-   Internet access on first configure, so CMake's `FetchContent` can
    pull in [`openai-cpp`](https://github.com/olrea/openai-cpp)
    (which vendors `nlohmann/json`) and, for tests,
    [Catch2](https://github.com/catchorg/Catch2)
-   Access to an OpenAI-compatible API endpoint

------------------------------------------------------------------------

## 🔧 Build

``` bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The `data_aggregator_analyzer` binary is produced in `build/`.

------------------------------------------------------------------------

## 🧪 Testing

Unit tests cover the pure logic (config parsing, CSV fence stripping,
JSON response extraction) and require no network access or API key.

``` bash
cmake -S . -B build -DDAA_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Pass `-DDAA_BUILD_TESTS=OFF` to skip building tests. CI runs this same
suite on every push and pull request via
[GitHub Actions](.github/workflows/ci.yml).

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

    ./build/data_aggregator_analyzer <audio_transcription_file> <video_semantic_file> [config.ini]

Example:

    ./build/data_aggregator_analyzer audio.txt video.txt config.ini

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

## 🤝 Contributing

Contributions are welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for build,
test, and pull request guidelines. Please also review our
[Code of Conduct](CODE_OF_CONDUCT.md).

Found a security issue? Please follow the responsible disclosure process in
[SECURITY.md](SECURITY.md) instead of opening a public issue.

------------------------------------------------------------------------

## 📜 License

GNU Affero General Public License v3.0 (AGPL-3.0-only)

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
