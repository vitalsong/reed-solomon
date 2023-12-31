cmake_minimum_required(VERSION 3.10)
project(reed-solomon-tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(NAME googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
        VERSION 1.13.0
    OPTIONS 
        "BUILD_GMOCK OFF"
        "gtest_force_shared_crt ON"
    EXCLUDE_FROM_ALL ON
)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC reed-solomon gtest)
