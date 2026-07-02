cmake_minimum_required(VERSION 4.2)
project(lfx_midi)

set(CMAKE_CXX_STANDARD 20)

add_library(lfx_midi STATIC library.cpp)
