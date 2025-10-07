# Author: Nick Waddoups
# Date: Oct 19, 2024
#
# Created for ECE 5720 - Computer Architecture - Branch Prediction Lab
#
# This is the CMake file used by the grader, so only change it if you are
# certain that the code will still be compile-able by the grader

cmake_minimum_required(VERSION 2.8)
project(cbp3)

# 32 for 32-bit system and 64 for 64-bit system
# It's very unlikely that you will ever need to set this to 32 in
# this day and age, but the option is still there :)
set(FORMAT 64)

# Include the original CBP3 precompiled library
include_directories(${CMAKE_SOURCE_DIR}/readerlib)
link_directories(${CMAKE_SOURCE_DIR}/readerlib)

# Create the cbp3 simulator and link it to precompiled library
add_executable(cbp3 cbp3_framework.cc predictor.cc)
target_link_libraries(cbp3 -Lreaderlib)
target_link_libraries(cbp3 -flto ${CMAKE_SOURCE_DIR}/readerlib/libcbp3_trace.64.a)
target_compile_options(cbp3 PUBLIC -std=c++11 -O3 -Wall -Ireaderlib -flto)
