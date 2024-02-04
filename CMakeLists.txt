cmake_minimum_required(VERSION 3.15)

project(json_pretty_print VERSION 0.0.1 LANGUAGES CXX)

set(SOURCES
    json_types.cpp
    json_parser.cpp
)

add_executable(json_pretty_print ${SOURCES})
