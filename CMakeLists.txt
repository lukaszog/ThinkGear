cmake_minimum_required(VERSION 3.12)
project(NeuroSky C)

set(CMAKE_C_STANDARD 99)

add_executable(NeuroSky main.c ThinkGearStreamParser.h)