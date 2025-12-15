/*
 *  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *
 *  Beeplayer - with Qt
 *  A lightweight and cross-platform music player based on miniaudio and C++.
 *
 *  Thanks to David Reid provided us a such powerful and useful lib "miniaudio"
 *  Thanks to The Qt Company provided us a Beautiful Cross-Platform UI Lib
 *
 *  JetBrains Provided us a free License
 *  Qt Company Provided us a free License
 *
 *  Thanks to music make the world so beautiful. :)
 */

// Standard Lib
#include <string>
#include <QApplication>

#ifdef _WIN32
#include <windows.h>
#endif

// Basic Lib
#include"miniaudio.c"
#include "UI/beeplayerui.h"
#include "Log/LogSystem.hpp"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Log::SetViewLogLevel(LogLevel::BP_INFO);

    // Win32 Platform Define
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_LOG, "Set the code page to CP_UTF8");
    #endif

    // Start Point
        BeeplayerUI *ui = nullptr;
        try {
                std::string rootPath;
                if (argc == 3 && std::string(argv[1]) == "-root") {
                    rootPath = argv[2];
                    ui = new BeeplayerUI(nullptr, rootPath);
                }else {
                    ui = new BeeplayerUI(nullptr, "");
                }

                if(ui) {
                    ui->show();
                }

        } catch (const std::exception& e) {
            Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_DEBUG, "Unknown Error:" , e.what());
        }

    return a.exec();
}
