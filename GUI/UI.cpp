/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: UI.cpp
 *  Lib: Beeplayer UI Provider with Dear ImGUI
 *  Author: Romi Brooks
 *  Date: 2025-07-03
 *  Type: GUI, Wrapper, UI
 */


#include "UI.hpp"
// Standard Lib
#include <iostream>
#include <vector>
#include <cstring>

// ImGUI Init
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Basic Lib
#include "../Log/LogSystem.hpp"

UI::UI(PlayerController& controller) : playerController(controller) {
    // 初始化GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // 创建窗口
    window = glfwCreateWindow(800, 600, "BeePlayer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_GLFW, "Failed to create GLFW window");
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 启用垂直同步

    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用键盘控制
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // 加载字体
    LoadFont();

    // 设置PlayerController的回调
    playerController.SetStateChangeCallback([this]() {
        trackChanged = true;
    });
}

UI::~UI() {
    // 清理ImGui和GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_IMGUI, "Exiting IMGUI&GLFW");

    if (window) {
        glfwDestroyWindow(window);
    }
    Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_GLFW, "Destroy Window");

    glfwTerminate();
    Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_GLFW, "glfwTerminate...");
}

void UI::LoadFont() {
    try {
        // 定义字符范围（包括中文和日文）
        static const ImWchar ranges[] = {
            // 基本拉丁字符
            0x0020, 0x00FF,
            // 平假名 (Hiragana)
            0x3040, 0x309F,
            // 片假名 (Katakana)
            0x30A0, 0x30FF,
            // 中日韩统一表意文字 (CJK Unified Ideographs)
            0x4E00, 0x9FFF,
            // 中日韩符号和标点
            0x3000, 0x303F,
            // 半角/全角形式
            0xFF00, 0xFFEF,
            // 终止符
            0,
        };

        ImFontConfig config;
        config.OversampleH = 2;
        config.OversampleV = 1;
        config.GlyphRanges = ranges;

        // 尝试加载字体文件
        font = io->Fonts->AddFontFromFileTTF("hwkt.ttf", 23.0f, &config, ranges);

        if (font) {
            io->FontDefault = font;
            Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_IMGUI, "Font Loaded!");

            // 重建字体纹理
            ImGui_ImplOpenGL3_DestroyFontsTexture();
            ImGui_ImplOpenGL3_CreateFontsTexture();
        } else {
            // 使用默认字体（不支持中文）
            io->Fonts->AddFontDefault();
            Log::LogOut(LogLevel::BP_WARNING, LogChannel::CH_IMGUI, "Can not load the font, using default!");
        }
    } catch (const std::exception& e) {
        Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_IMGUI, "Error when loading the font: " + std::string(e.what()));
        // 回退到默认字体
        io->Fonts->AddFontDefault();
    }
}

std::string UI::ShowPathSelection() {
    std::string rootPath;
    bool pathSelected = false;

    while (!glfwWindowShouldClose(window) && !pathSelected) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io->DisplaySize);
        ImGui::Begin("选择音乐目录", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove);

        ImGui::Text("欢迎使用 BeePlayer");
        ImGui::Separator();

        static char pathBuffer[256] = "";
        ImGui::InputText("音乐目录路径", pathBuffer, IM_ARRAYSIZE(pathBuffer));

        if (ImGui::Button("确认", ImVec2(100, 40))) {
            rootPath = pathBuffer;
            pathSelected = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("退出", ImVec2(100, 40))) {
            glfwSetWindowShouldClose(window, true);
            pathSelected = true;
        }

        ImGui::End();

        RenderFrame();
    }

    return rootPath;
}

void UI::RenderMainUI() const {
    // 检查歌曲切换标记
    if (trackChanged) {
        trackChanged = false;
        // 强制刷新当前窗口（通过设置焦点，ImGui会自动刷新）
        ImGui::SetWindowFocus("BeePlayer");
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 创建播放器窗口
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("BeePlayer", nullptr, ImGuiWindowFlags_NoCollapse);

    // 显示当前歌曲
    ImGui::Text("当前播放: %s", playerController.GetCurrentTrack().c_str());
    ImGui::Separator();

	// 更新进度
	playerController.UpdateProgress();

	// 获取当前进度
	float progress = playerController.GetProgress();

	// 显示进度条
	ImGui::Text("进度");
	if (ImGui::SliderFloat("##seek_bar", &progress, 0.0f, 1.0f, "%.2f")) {
		playerController.SeekToPosition(progress);
	}

    // 控制按钮区域
    RenderControlButtons();

    // 音量控制
    ImGui::Separator();
    ImGui::Text("音量");
    float volume = playerController.GetVolume();
    if (ImGui::SliderFloat("##volume", &volume, 0.0f, 1.0f, "%.2f")) {
        playerController.SetVolume(volume);
    }

    // 播放列表
    RenderPlaylist();

    ImGui::End();

    RenderFrame();
}

void UI::RenderControlButtons() const {
    ImGui::BeginGroup();

    // 上一曲按钮
    if (ImGui::Button("上一首", ImVec2(80, 40))) {
        playerController.Prev();
    }

    ImGui::SameLine();

    // 播放/暂停按钮
    if (ImGui::Button(playerController.IsPlaying() ? "暂停" : "播放", ImVec2(80, 40))) {
        if (playerController.IsPlaying()) {
            playerController.Pause();
        } else {
            playerController.Play();
        }
    }

    ImGui::SameLine();

    // 停止按钮
    if (ImGui::Button("停止", ImVec2(80, 40))) {
        playerController.Stop();
    }

    ImGui::SameLine();

    // 下一曲按钮
    if (ImGui::Button("下一首", ImVec2(80, 40))) {
        playerController.Next();
    }

    ImGui::EndGroup();
}

void UI::RenderPlaylist() const {
    ImGui::Separator();
    ImGui::Text("播放列表 (共%d首歌曲)", playerController.GetTracks().size());

    // 获取当前曲目索引
    int currentTrackIndex = static_cast<int>(playerController.GetCurrentTrackIndex());
    
    // 转换为ImGui需要的格式
    const auto& tracks = playerController.GetTracks();
    std::vector<const char*> trackPointers;
    for (const auto& track : tracks) {
        trackPointers.push_back(track.c_str());
    }

    // 显示播放列表
    if (ImGui::ListBox("##tracks", &currentTrackIndex,
                      trackPointers.data(), static_cast<int>(trackPointers.size()), 8))
    {
        // 当用户选择列表中的某一项时，切换到该曲目
        if (currentTrackIndex >= 0 && currentTrackIndex < static_cast<int>(tracks.size())) {
        	playerController.Switch(static_cast<size_t>(currentTrackIndex));
        }
    }
}

void UI::RenderFrame() const {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void UI::Run(const std::string &rootPath) {
	std::string selectedPath = rootPath;

	// 如果没有通过参数提供路径，显示路径选择对话框
	if (selectedPath.empty()) {
		selectedPath = ShowPathSelection();
	}

	if (selectedPath.empty() || glfwWindowShouldClose(window)) {
		return;
	}

	// 初始化播放器
	if (!playerController.Initialize(selectedPath)) {
		Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_CONTROLLER,
				   "Failed to initialize audio player.");
		return;
	}

	// 主循环
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		RenderMainUI();
	}
}