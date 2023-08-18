cmake_minimum_required(VERSION 3.10)
project(reed-solomon-benchs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cpmaddpackage(NAME benchmark
  GIT_REPOSITORY 
    "https://github.com/google/benchmark.git"
  VERSION 
    "1.8.2"
  OPTIONS
    "BENCHMARK_ENABLE_TESTING OFF"
    "HAVE_POSIX_REGEX OFF"
    "HAVE_STD_REGEX OFF"
    "BENCHMARK_ENABLE_INSTALL OFF"
    "BENCHMARK_INSTALL_DOCS OFF"
)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} reed-solomon benchmark)
