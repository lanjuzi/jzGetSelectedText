cmake_minimum_required(VERSION 3.22)
project(JZGetSelectedText C)

set(CMAKE_C_STANDARD 11)

add_library(JZGetSelectedText SHARED jzGetSelectedText.c)

add_executable(Example example.c jzGetSelectedText.c)
