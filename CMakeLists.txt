cmake_minimum_required(VERSION 3.15)

project(json_pretty_print VERSION 0.0.1 LANGUAGES CXX)

set(SOURCES
    main.cpp
    json_types.cpp
)

add_executable(json_pretty_print ${SOURCES})
