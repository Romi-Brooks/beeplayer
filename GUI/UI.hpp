/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: UI.hpp
 *  Lib: Beeplayer UI Provider with Dear ImGUI definitions
 *  Author: Romi Brooks
 *  Date: 2025-07-03
 *  Type: GUI, Wrapper, UI
 */

#ifndef UI_HPP
#define UI_HPP

// Standard Lib
#include <string>

// ImGUI Init
#include <GLFW/glfw3.h>
#include <imgui.h>

// Basic Lib
#include "../Engine/Controller.hpp"

class UI {
public:
	explicit UI(PlayerController& controller);
	~UI();

	// 禁用拷贝
	UI(const UI&) = delete;
	UI& operator=(const UI&) = delete;

	// 运行UI主循环
	void Run(const std::string& rootPath = "");

private:
	void LoadFont();
	std::string ShowPathSelection();
	void RenderMainUI() const;
	void RenderControlButtons() const;
	void RenderPlaylist() const;
	void RenderFrame() const;


	GLFWwindow* window = nullptr;
	ImGuiIO* io = nullptr;
	ImFont* font = nullptr;
	PlayerController& playerController;

	mutable bool trackChanged = false;

	std::chrono::steady_clock::time_point lastRenderTime;
	const float maxFPS = 60.0f; // 最大帧率
};

#endif // UI_HPP