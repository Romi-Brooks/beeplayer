/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: UI.hpp
 *  Lib: Beeplayer Graph User Interface (aka.GUI) definitions -> GUI Basic Lib
 *  Author: Romi Brooks
 *  Date: 2025-06-1
 *  Type: GUI
 */

#ifndef UI_HPP
#define UI_HPP

#include <string>

// ImGUI & glfw render lib
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include "../Engine/MusicPlayerState.hpp"

class UI {
public:
	UI(MusicPlayerState& state);

    ~UI();

    // 加载字体
    void LoadFont();

    // 显示路径选择界面
    std::string ShowPathSelection() const;

    // 更新进度条
    void UpdateProgress() const;

    // 渲染主界面
    void RenderMainUI() const;

    // 渲染控制按钮
    void RenderControlButtons() const;

    // 渲染播放列表
    void RenderPlaylist() const;

    // 渲染帧
    void RenderFrame() const;

    // 主循环
    void Run() const;

    GLFWwindow* GetWindow() const { return window; }

private:
    GLFWwindow* window = nullptr;
    ImGuiIO* io = nullptr;
    ImFont* font = nullptr;
    MusicPlayerState& playerState;
};


#endif //UI_HPP
