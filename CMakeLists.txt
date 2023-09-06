cmake_minimum_required(VERSION 3.4)
project(reed-solomon LANGUAGES C CXX VERSION 0.0.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(REED_SOLOMON_BUILD_TESTS "Build tests" OFF)
option(REED_SOLOMON_BUILD_BENCHS "Build benchs" OFF)
option(REED_SOLOMON_ASAN_ENABLED "Address sanitizer enabled" OFF)

set(SOURCES
    src/reed-solomon.cpp
    src/rslib/rslib.c
)

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src/rslib)
target_include_directories(${PROJECT_NAME} PUBLIC include)

if (REED_SOLOMON_ASAN_ENABLED)
    include(cmake/sanitizer.cmake)
    enable_address_sanitizer(TARGET ${PROJECT_NAME})
endif()

if (REED_SOLOMON_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (REED_SOLOMON_BUILD_BENCHS)
    add_subdirectory(benchs)
endif()