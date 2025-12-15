cmake_minimum_required(VERSION 3.28)

project(beeplayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(
        miniaudio
        Engine
        Log
        FileSystem
)

set(UI_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/UI/beeplayerui.ui"
)

add_definitions(-DTAGLIB_STATIC)
get_filename_component(TAGLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/FileSystem/taglib" ABSOLUTE)
message(STATUS "TAGLIB_ROOT: ${TAGLIB_ROOT}")

# 使用绝对路径直接设置变量（避免 find_path 失败）
set(TAGLIB_INCLUDE_DIR "${TAGLIB_ROOT}/include")
set(TAGLIB_LIBRARY "${TAGLIB_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}tag${CMAKE_STATIC_LIBRARY_SUFFIX}")

message(STATUS "TAGLIB_INCLUDE_DIR: ${TAGLIB_INCLUDE_DIR}")
message(STATUS "TAGLIB_LIBRARY: ${TAGLIB_LIBRARY}")

set(PROJECT_SOURCES
        #       miniaudio
                miniaudio/miniaudio.h
                miniaudio/miniaudio.c
        #       Beeplayer Main prog
                beeplayer.cpp
        #       Abstract Wrapper
                Engine/Device.cpp
                Engine/Decoder.cpp
                Engine/Player.cpp
                Engine/Buffering.cpp
                Engine/Status.cpp
                Engine/Controller.cpp
                Engine/Controller.hpp
                Engine/DataCallback.cpp
                Engine/DataCallback.hpp
        #       Module Load
                Log/LogSystem.cpp
                FileSystem/Path.cpp
                FileSystem/Path.hpp
                FileSystem/Encoding.cpp
                FileSystem/Encoding.hpp
                FileSystem/Metadata.cpp
                FileSystem/Metadata.hpp
        #       UI Provided
                ${UI_HEADERS}
                UI/beeplayerui.h
                UI/beeplayerui.cpp
                UI/Animations/rotatingalbumart.h
                UI/Animations/rotatingalbumart.cpp
                UI/Animations/scrolllabel.h UI/Animations/scrolllabel.cpp
                UI/volumeslider.h
                UI/volumeslider.cpp
                UI/progresswidget.h
                UI/progresswidget.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(beeplayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Resource/Resource.qrc
    )

if(WIN32)
    # 添加图标资源文件到可执行目标
    target_sources(beeplayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc)
endif()

set_target_properties(beeplayer PROPERTIES
    WIN32_EXECUTABLE TRUE
    RC_ICONS "app_icon.ico"  # 去掉路径前缀
)

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET beeplayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(beeplayer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(beeplayer
            ${PROJECT_SOURCES}
        )
    endif()
endif()



# 链接 TagLib - 在 Qt 之前链接
target_include_directories(beeplayer PRIVATE ${TAGLIB_INCLUDE_DIR})
target_link_libraries(beeplayer PRIVATE ${TAGLIB_LIBRARY})

target_link_libraries(beeplayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_link_libraries(beeplayer PRIVATE
    ${TAGLIB_LIBRARY}
    z
    ole32
    uuid
    winmm
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.beeplayer)
endif()
set_target_properties(beeplayer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS beeplayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(beeplayer)
endif()
