cmake_minimum_required(VERSION 3.28)
project(beeplayer)

set(CMAKE_CXX_STANDARD 20)

include_directories(imgui/glfw/include
                    imgui/
                    imgui/backends)

link_directories(imgui/glfw/lib-mingw-w64)

file(GLOB_RECURSE IMGUI_SRCS imgui/*.cpp)

add_executable(beeplayer miniaudio/miniaudio.c
#        beeplayer.cpp
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
        ${IMGUI_SRCS}
         GUI/GUI.cpp
)

# Static linking to exec file
if(MINGW)
    # 静态链接C/C++标准库
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

    # libwinpthread-1.dll
    find_package(Threads REQUIRED)
    target_link_libraries(beeplayer PRIVATE Threads::Threads glfw3.a opengl32)

    # 强制静态链接标准库（双重保险）
    target_link_options(beeplayer PRIVATE -static-libgcc -static-libstdc++)
endif()