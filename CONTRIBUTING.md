# Contributing

Thanks for your interest in improving the OR-Edge Data Aggregator & Analyzer.

## Getting started

### Requirements

- A C++20 compiler
- CMake 3.14+
- `libcurl` development headers (e.g. `libcurl4-openssl-dev` on Debian/Ubuntu,
  `curl` via Homebrew on macOS)
- Internet access on first configure, so CMake's `FetchContent` can pull in
  [openai-cpp](https://github.com/olrea/openai-cpp) and
  [Catch2](https://github.com/catchorg/Catch2) (both are cached under
  `build/_deps` afterwards)

### Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### Run the tests

Unit tests live in [tests/](tests/) and cover the pure logic in
[core.hpp](core.hpp)/[core.cpp](core.cpp) (config parsing, CSV fence
stripping, JSON response extraction) — no network or API key required.

```bash
cmake -S . -B build -DDAA_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Set `-DDAA_BUILD_TESTS=OFF` to skip building tests (e.g. for a minimal
release build).

## Making changes

- Keep new, reusable, side-effect-free logic in `core.hpp`/`core.cpp` so it
  stays unit-testable; keep `data_aggregator_analyzer.cpp` focused on wiring
  up the OpenAI-compatible client and I/O.
- Add or update unit tests in `tests/test_core.cpp` for any behavior change.
- Match the existing code style (brace placement, `static`/free-function
  usage, naming) in the file you're editing.
- New source files should carry the same AGPL-3.0 license header used in the
  existing files.
- Run the full build and test suite before opening a pull request.

## Submitting a pull request

1. Fork the repository and create a branch from `main`.
2. Make your changes, with tests where applicable.
3. Ensure `cmake --build build` and `ctest --test-dir build` both pass.
4. Open a pull request describing the change and the motivation behind it.

## Reporting bugs and requesting features

Please use the issue templates under **Issues → New issue**.

## Security issues

Do not open a public issue for security vulnerabilities — see
[SECURITY.md](SECURITY.md) instead.
