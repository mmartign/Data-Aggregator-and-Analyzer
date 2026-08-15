cmake_minimum_required(VERSION 3.14)
project(DataAggregatorAnalyzer LANGUAGES CXX)

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DAA_BUILD_TESTS "Build unit tests for Data Aggregator Analyzer" ON)

include(FetchContent)

# Header-only OpenAI-compatible client (also vendors nlohmann/json). We only
# need its headers, so populate the sources without add_subdirectory-ing its
# CMakeLists.txt (which would otherwise pull in its unrelated examples).
FetchContent_Declare(
    openai_cpp
    GIT_REPOSITORY https://github.com/olrea/openai-cpp.git
    GIT_TAG 9554a4d86d7650bb3b46db772dbacd5f3e054b8c
)
FetchContent_GetProperties(openai_cpp)
if(NOT openai_cpp_POPULATED)
    FetchContent_Populate(openai_cpp)
endif()

# Core library: pure logic (config parsing, CSV/JSON helpers, file I/O,
# logging) with no dependency on libcurl, so it can be built and unit tested
# in isolation. Split into one module per functional area rather than a
# single monolithic core.
add_library(daa_core STATIC
    config.cpp config.hpp
    file_io.cpp file_io.hpp
    logging.cpp logging.hpp
    openai_response.cpp openai_response.hpp
    text_utils.cpp text_utils.hpp
)
target_include_directories(daa_core PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${openai_cpp_SOURCE_DIR}/include/openai
)

# executable
add_executable(data_aggregator_analyzer data_aggregator_analyzer.cpp)
target_include_directories(data_aggregator_analyzer PRIVATE
    ${openai_cpp_SOURCE_DIR}/include/openai
)
target_link_libraries(data_aggregator_analyzer PRIVATE daa_core)

# link libraries
find_package(CURL REQUIRED)
if(CURL_FOUND)
    target_include_directories(data_aggregator_analyzer PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(data_aggregator_analyzer PRIVATE ${CURL_LIBRARIES})
else()
    message(FATAL_ERROR "CURL library not found")
endif()

# install rules (optional)
install(TARGETS data_aggregator_analyzer DESTINATION bin)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(DAA_BUILD_TESTS)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )
    FetchContent_MakeAvailable(catch2)

    enable_testing()

    add_executable(daa_tests
        tests/test_config.cpp
        tests/test_file_io.cpp
        tests/test_openai_response.cpp
        tests/test_text_utils.cpp
    )
    target_link_libraries(daa_tests PRIVATE daa_core Catch2::Catch2WithMain)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(CTest)
    include(Catch)
    catch_discover_tests(daa_tests)
endif()
