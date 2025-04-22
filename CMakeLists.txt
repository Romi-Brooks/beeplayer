cmake_minimum_required(VERSION 3.28)
project(beeplayer)

set(CMAKE_CXX_STANDARD 20)

add_executable(beeplayer beeplayer.cpp
        Engine/AudioDevice.cpp
        Engine/AudioDevice.hpp
        Engine/AudioDecoder.cpp
        Engine/AudioDecoder.hpp
        miniaudio/miniaudio.c
)
