cmake_minimum_required(VERSION 3.0)

project(skyskrapers LANGUAGES C)

add_compile_options(-g -O3 -fPIC)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic

    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wenum-compare
    -Wfloat-equal
    -Wredundant-decls
    -Wsign-conversion
)

add_subdirectory(src)
