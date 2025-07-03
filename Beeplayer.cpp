
/*
 *  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *
 *  Beeplayer - with ImGUI
 *  A lightweight and cross-platform music player based on miniaudio and C++.
 *
 *  Thanks to David Reid provided us a such powerful and useful lib "miniaudio"
 *	Thanks to Omar Cornut provided us a lightweight and simple GUI Lib "Dear ImGUI"
 *	And I use GLFW + OpenGL for the backend
 *
 *  Thanks to music make the world so beautiful. :)
 */

// Standard Lib
#include <iostream>
#include <thread>
#include <string>
#include <complex>

#ifdef _WIN32
#include <windows.h>
#endif

// ImGUI Init
#include "GUI/UI.hpp"

// Basic Lib
#include "Engine/Controller.hpp"
#include "FileSystem/Path.hpp"
#include "Log/LogSystem.hpp"

int main(int argc, char** argv) {
	Log::SetViewLogLevel(LogLevel::BP_INFO);

	#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
		Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_LOG, "Set the code page to CP_UTF8");
	#endif

	try {
		PlayerController playerController;
		UI gui(playerController);

		std::string rootPath;
		if (argc == 3 && std::string(argv[1]) == "-root") {
			rootPath = argv[2];
		}

		gui.Run(rootPath);
	} catch (const std::exception& e) {
		Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_DEBUG, "Unknown Error:" , e.what());
	}

	return 0;
}