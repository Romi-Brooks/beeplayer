cmake_minimum_required(VERSION 3.28)
project(beeplayer)

set(CMAKE_CXX_STANDARD 20)

add_executable(beeplayer miniaudio/miniaudio.c
        beeplayer.cpp
        Engine/AudioDevice.cpp
        Engine/AudioDevice.hpp
        Engine/AudioDecoder.cpp
        Engine/AudioDecoder.hpp
        FileSystem/Path.cpp
        FileSystem/Path.hpp
        Engine/Player.cpp
        Engine/Player.hpp
)
