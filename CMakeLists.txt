cmake_minimum_required(VERSION 3.15)

project(json_pretty_print VERSION 0.0.1 LANGUAGES CXX)

set(SOURCES
    runtime_polymorphism_source/run_tests.cpp
    runtime_polymorphism_source/json_types.cpp
)

add_executable(run_tests ${SOURCES})
