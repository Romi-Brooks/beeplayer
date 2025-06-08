cmake_minimum_required(VERSION 3.28)
project(beeplayer)

set(CMAKE_CXX_STANDARD 20)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(
        imgui/glfw/include
        imgui
        imgui/backends
        miniaudio
        Engine
        Log
        FileSystem
)

# 收集源文件
file(GLOB_RECURSE IMGUI_SRCS
        "imgui/*.cpp"
        "imgui/backends/imgui_impl_glfw.cpp"
        "imgui/backends/imgui_impl_opengl3.cpp"
)

# 添加可执行文件
add_executable(beeplayer
#       miniaudio Lib
        miniaudio/miniaudio.c
#       Beeplayer Main prog
        Beeplayer.cpp
#       Abstract Wrapper
        Engine/Device.cpp
        Engine/Decoder.cpp
        Engine/Player.cpp
        Engine/Buffering.cpp
        Engine/Status.cpp
        Engine/MusicPlayerState.cpp
        Engine/MusicPlayerState.hpp
        Engine/DataCallback.cpp
        Engine/DataCallback.hpp
        Log/LogSystem.cpp
        FileSystem/Path.cpp
        FileSystem/Path.hpp
        FileSystem/Encoding.cpp
        FileSystem/Encoding.hpp
#       GUI Provided
        ${IMGUI_SRCS}
        GUI/UI.cpp
        GUI/UI.hpp

)

# 设置静态链接选项
if(MINGW)
    # 静态链接选项
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    target_link_options(beeplayer PRIVATE -static-libgcc -static-libstdc++)
endif()

# 查找 GLFW 库
find_library(GLFW_LIBRARY
        NAMES glfw3 glfw
        PATHS ${CMAKE_SOURCE_DIR}/imgui/glfw/lib-mingw-w64
        NO_DEFAULT_PATH
)

if(GLFW_LIBRARY)
    message(STATUS "找到 GLFW 库: ${GLFW_LIBRARY}")
    # 使用关键字签名链接 GLFW
    target_link_libraries(beeplayer PRIVATE ${GLFW_LIBRARY})
else()
    message(FATAL_ERROR "未找到 GLFW 库！请检查路径")
endif()

# 链接 Windows 平台需要的其他库 - 使用关键字签名
target_link_libraries(beeplayer PRIVATE
        opengl32
        gdi32
)

# 链接系统库 - 使用关键字签名
target_link_libraries(beeplayer PRIVATE
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
)

# 设置目标属性
set_target_properties(beeplayer PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)