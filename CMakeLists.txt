cmake_minimum_required(VERSION 3.10)
project(DataAggregatorAnalyzer LANGUAGES CXX)

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# include directories
include_directories(${CMAKE_SOURCE_DIR}/../openai-cpp/include/openai)

# executable
add_executable(data_aggregator_analyzer data_aggregator_analyzer.cpp)

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
