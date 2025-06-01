/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: UI.cpp
 *  Lib: Beeplayer Graph User Interface (aka.GUI) -> GUI Basic
 *  Author: Romi Brooks
 *  Date: 2025-06-1
 *  Type: GUI
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cstring>

#include "UI.hpp"

#include <imgui_impl_opengl3.h>

#include "../miniaudio/miniaudio.h"
#include "../Engine/Player.hpp"
#include "../Log/LogSystem.hpp"
#include "../FileSystem/Path.hpp"
#include "../Engine/DataCallback.hpp"

UI::UI(MusicPlayerState &state)  : playerState(state) {
	// 初始化GLFW
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	// 创建窗口
	window = glfwCreateWindow(800, 600, "BeePlayer", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // 启用垂直同步

	// 初始化ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	// 加载字体
	LoadFont();
}

UI::~UI()  {
	// 清理ImGui和GLFW
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	if (window) {
		glfwDestroyWindow(window);
	}

	if (playerState.initialized) {
		std::lock_guard<std::mutex> lock(playerState.audioMutex);
		ma_device_stop(&playerState.Device->GetDevice());
		playerState.Timer->ResetStatus();
		playerState.progress = 0.0f;
		playerState.isPlaying = false;
	}
	CleanupAudioPlayer(playerState);

	glfwTerminate();
}

void UI::LoadFont()  {
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
			LOG_INFO("ImGUI -> Font Loaded!");

			// 重建字体纹理
			ImGui_ImplOpenGL3_DestroyFontsTexture();
			ImGui_ImplOpenGL3_CreateFontsTexture();
		} else {
			// 使用默认字体（不支持中文）
			io->Fonts->AddFontDefault();
			LOG_WARNING("ImGUI -> Can not load the font, using default!");
		}
	} catch (const std::exception& e) {
		std::cerr << "ImGUI -> 字体加载错误: " << e.what() << std::endl;
		// 回退到默认字体
		io->Fonts->AddFontDefault();
	}
}

std::string UI::ShowPathSelection() const  {
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

void UI::UpdateProgress() const  {
	if (playerState.initialized && playerState.Timer) {
		double current = playerState.Buffer->GetGlobalFrameCount() / playerState.Decoder->GetDecoder().outputSampleRate;
		double total = playerState.Timer->GetTotalFrames() / playerState.Decoder->GetDecoder().outputSampleRate;

		if (total > 0) {
			playerState.progress = static_cast<float>(current / total);
		} else {
			playerState.progress = 0.0f;
		}
	}
}

void UI::RenderMainUI() const {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 创建播放器窗口
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("BeePlayer", nullptr, ImGuiWindowFlags_NoCollapse);

	// 显示当前歌曲
	if (playerState.initialized && !playerState.tracks.empty() &&
		playerState.currentTrack < playerState.tracks.size()) {
		ImGui::Text("当前播放: %s", playerState.tracks[playerState.currentTrack].c_str());
		} else {
			ImGui::Text("当前播放: 无");
		}
	ImGui::Separator();

	// 播放进度条
	UpdateProgress();
	ImGui::Text("进度");
	ImGui::ProgressBar(playerState.progress, ImVec2(-1, 25));

	// 控制按钮区域
	RenderControlButtons();

	// 音量控制
	ImGui::Separator();
	ImGui::Text("音量控制");
	if (ImGui::SliderFloat("##volume", &playerState.volume, 0.0f, 1.0f, "%.1f")) {
		// 在实际应用中设置音量
		if (playerState.initialized) {
		    ma_device_set_master_volume(&playerState.Device->GetDevice(), playerState.volume);
		}
	}

	// 播放列表
	RenderPlaylist();

	ImGui::End();

	RenderFrame();
}

void UI::RenderControlButtons() const  {
        ImGui::BeginGroup();

        // 上一曲按钮
        if (ImGui::Button("上一曲", ImVec2(80, 40))) {
            if (playerState.initialized) {
                std::lock_guard<std::mutex> lock(playerState.audioMutex);
                playerState.Player->Switch(
                    *playerState.Pather, *playerState.Decoder,
                    *playerState.Device, data_callback, *playerState.Timer,
                    *playerState.Buffer, SwitchAction::PREV
                );
                playerState.currentTrack = playerState.Pather->Index();
            }
        }

        ImGui::SameLine();

        // 播放/暂停按钮
        if (ImGui::Button(playerState.isPlaying ? "暂停" : "播放", ImVec2(80, 40))) {
            if (playerState.initialized) {
                std::lock_guard<std::mutex> lock(playerState.audioMutex);
                if (ma_device_is_started(&playerState.Device->GetDevice())) {
                    ma_device_stop(&playerState.Device->GetDevice());
                    playerState.isPlaying = false;
                } else {
                    ma_device_start(&playerState.Device->GetDevice());
                    playerState.isPlaying = true;
                }
            }
        }

        ImGui::SameLine();

        // 停止按钮
        if (ImGui::Button("停止", ImVec2(80, 40))) {
            if (playerState.initialized) {
                std::lock_guard<std::mutex> lock(playerState.audioMutex);
                ma_device_stop(&playerState.Device->GetDevice());
                playerState.Timer->ResetStatus();
                playerState.progress = 0.0f;
                playerState.isPlaying = false;
            }
        }

        ImGui::SameLine();

        // 下一曲按钮
        if (ImGui::Button("下一曲", ImVec2(80, 40))) {
            if (playerState.initialized) {
                std::lock_guard<std::mutex> lock(playerState.audioMutex);
                playerState.Player->Switch(
                    *playerState.Pather, *playerState.Decoder,
                    *playerState.Device, data_callback, *playerState.Timer,
                    *playerState.Buffer, SwitchAction::NEXT
                );
                playerState.currentTrack = playerState.Pather->Index();
            }
        }

        ImGui::EndGroup();
    }

void UI::RenderPlaylist() const {
	ImGui::Separator();
	ImGui::Text("播放列表 (共%d首歌曲)", playerState.tracks.size());

	// 转换为ImGui需要的格式
	std::vector<const char*> trackPointers;
	for (const auto& track : playerState.tracks) {
		trackPointers.push_back(track.c_str());
	}

	if (ImGui::ListBox("##tracks", &playerState.currentTrack,
					  trackPointers.data(), trackPointers.size(), 8))
	{
		if (playerState.initialized) {
			std::lock_guard<std::mutex> lock(playerState.audioMutex);

			// 保存当前索引
			size_t currentIndex = playerState.Pather->Index();

			// 设置新索引
			playerState.Pather->SetIndex(playerState.currentTrack);

			// // 如果新索引大于当前索引，使用 NEXT 动作
			// // 如果小于，使用 PREV 动作
			// SwitchAction action = (playerState.currentTrack > currentIndex)
			// 					 ? SwitchAction::NEXT : SwitchAction::PREV;

			// 调用切换函数
			playerState.Player->Switch(
				*playerState.Pather, *playerState.Decoder,
				*playerState.Device, data_callback, *playerState.Timer,
				*playerState.Buffer, SPECIFIC
			);

			playerState.isPlaying = true;
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

void UI::Run() const {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		RenderMainUI();
	}
}
