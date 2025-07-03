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
	UI(PlayerController& controller);
	~UI();

	// 禁用拷贝
	UI(const UI&) = delete;
	UI& operator=(const UI&) = delete;

	// 运行UI主循环
	void Run(const std::string& rootPath = "");

private:
	// 辅助方法
	void LoadFont();
	std::string ShowPathSelection();
	void RenderMainUI() const;
	void RenderControlButtons() const;
	void RenderPlaylist() const;
	void RenderFrame() const;

	// 成员变量
	PlayerController& playerController;
	GLFWwindow* window = nullptr;
	ImGuiIO* io = nullptr;
	ImFont* font = nullptr;

	// 用于回调的状态标记
	mutable bool trackChanged = false;
};

#endif // UI_HPP