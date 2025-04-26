cmake_minimum_required(VERSION 3.28)
project(beeplayer)

set(CMAKE_CXX_STANDARD 20)

add_executable(beeplayer miniaudio/miniaudio.c beeplayer.cpp
        Engine/Device.cpp
        Engine/Device.hpp
        Engine/Decoder.cpp
        Engine/Decoder.hpp
        Engine/Player.cpp
        Engine/Player.hpp
        Engine/Buffering.cpp
        Engine/Buffering.hpp
        Engine/Status.cpp
        Engine/Status.hpp
        Log/LogSystem.cpp
        Log/LogSystem.hpp
        FileSystem/Path.cpp
        FileSystem/Path.hpp
)
