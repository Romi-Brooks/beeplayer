/*
 *  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *
 *  Beeplayer - with ImGUI
 *  A lightweight and cross-platform music player based on miniaudio and C++.
 *
 *  Thanks to David Reid provided us a such powerful and useful lib "miniaudio"
 *
 *  Thanks to music make the world so beautiful. :)
 *  and happy Children's Day - pub on 6-1-2025
 */

// Standard Lib
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>

// ImGUI Init
#include "GUI/UI.hpp"

// Basic Lib
#include "miniaudio/miniaudio.h"
#include "Engine/MusicPlayerState.hpp"
#include "Log/LogSystem.hpp"
#include "FileSystem/Path.hpp"

int main(int argc, char** argv) {
	// Global Audio Player Controller
	MusicPlayerState PlayerState;

	try {
		// give me an ui entity
		UI gui(PlayerState);

		// Get the Music path
		std::string rootPath;
		if (argc == 3 && std::string(argv[1]) == "-root") {
			rootPath = argv[2];
		} else {
			// Render File Selector
			rootPath = gui.ShowPathSelection();
		}

		// If close the window and selected no path
		if (rootPath.empty() && glfwWindowShouldClose(gui.GetWindow())) {
			return 0;
		}

		// Init the Music Player
		if (!rootPath.empty() && !InitializeAudioPlayer(PlayerState, rootPath)) {
			LOG_ERROR("PlayerState -> Can not Init the Audio Player!");
			return 1;
		}

		// 运行主界面
		gui.Run();
	} catch (const std::exception& e) {
		std::stringstream ss;
		ss << "PlayerState - > Error when load path: " << e.what() << std::endl;
		LOG_ERROR(ss.str());
		return 1;
	}

	// 清理音频资源
	CleanupAudioPlayer(PlayerState);

	return 0;
}