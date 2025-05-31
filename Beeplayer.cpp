#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <string>
#include <cstring>

// miniaudio 相关头文件
#include "miniaudio/miniaudio.h"
#include "Engine/Buffering.hpp"
#include "Engine/Decoder.hpp"
#include "Engine/Device.hpp"
#include "Engine/Player.hpp"
#include "Engine/Status.hpp"
#include "Log/LogSystem.hpp"
#include "FileSystem/Path.hpp"

// 音频播放器全局状态
struct MusicPlayerState {
    std::vector<std::string> tracks;
    int currentTrack = 0;
    std::atomic<bool> isPlaying{false};
    std::atomic<float> progress{0.0f};
    float volume = 0.8f;
    
    // 音频相关对象
    Path* pather = nullptr;
    AudioDecoder* decoder = nullptr;
    AudioDevice* device = nullptr;
    AudioPlayer* player = nullptr;
    Status* timer = nullptr;
    AudioBuffering* buffer = nullptr;
    
    // 线程控制
    std::atomic<bool> running{true};
    std::thread nextCheckThread;
    std::thread playThread;
    
    // 互斥锁
    std::mutex audioMutex;
    
    // 状态标志
    bool initialized = false;
};

// 修改后的回调函数
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    auto* buffering = static_cast<AudioBuffering*>(pDevice->pUserData);
    static ma_uint64 consumedFrames = 0;

    const int currentBufIdx = buffering->GetActiveBuffer();
    AudioBuffering::Buffer& currentBuf = buffering->GetBuffers()[currentBufIdx];

    if (!currentBuf.s_ready) {
        memset(pOutput, 0, frameCount * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels));
        return;
    }

    const ma_uint64 availableFrames = currentBuf.s_totalFrames - consumedFrames;
    const ma_uint64 framesToCopy = std::min(static_cast<ma_uint64>(frameCount), availableFrames);

    const size_t bytesToCopy = framesToCopy * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
    memcpy(
        pOutput,
        currentBuf.s_data.data() + consumedFrames * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels),
        bytesToCopy
    );

    consumedFrames += framesToCopy;
    buffering->ConsumeFrames(framesToCopy);

    if (consumedFrames >= currentBuf.s_totalFrames) {
        currentBuf.s_ready = false;
        buffering->SwitchBuffer();
        consumedFrames = 0;
    }

    if (framesToCopy < frameCount) {
        const size_t remainingBytes = (frameCount - framesToCopy) * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
        memset(static_cast<char*>(pOutput) + bytesToCopy, 0, remainingBytes);
    }
}

// 初始化音频播放器
bool InitializeAudioPlayer(MusicPlayerState& state, const std::string& rootPath) {
    std::lock_guard<std::mutex> lock(state.audioMutex);
    
    try {
        // 创建音频相关对象
        state.pather = new Path(rootPath);
        state.decoder = new AudioDecoder();
        state.device = &AudioDevice::GetDeviceInstance();
        state.player = new AudioPlayer();
        
        // 初始化解码器
        state.player->InitDecoder(*state.pather, *state.decoder);
        
        // 创建状态计时器 - 修复: 传递decoder参数
        state.timer = new Status(*state.decoder);
        
        // 创建双缓冲
        state.buffer = new AudioBuffering(&state.decoder->GetDecoder());
        
        // 初始化设备
        state.player->InitDevice(*state.decoder, *state.device, data_callback, *state.buffer);
        
        // 获取当前目录的音乐文件
        auto files = state.pather->GetFiles();
        for (const auto& file : files) {
            state.tracks.push_back(file);
        }
        
        if (state.tracks.empty()) {
            LOG_ERROR("No Media File founded!");
            return false;
        }
        
        // 启动后台线程
        state.nextCheckThread = std::thread([&] {
            while (state.running) {
                if (state.initialized && state.player && state.buffer && state.timer && 
                    state.pather && state.decoder && state.device) {
                    state.player->NextFileCheck(
                        *state.buffer, *state.timer, *state.pather, 
                        *state.decoder, *state.device, data_callback
                    );
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        });
        
        state.playThread = std::thread([&] {
            if (state.initialized && state.player && state.device && 
                state.decoder && state.timer && state.buffer) {
                state.player->Play(
                    state.device->GetDevice(), 
                    state.decoder->GetDecoder(), 
                    *state.timer, 
                    *state.buffer
                );
            }
        });
        
        state.initialized = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "ImGUI -> 音频播放器初始化失败: " << e.what() << std::endl;
        return false;
    }
}

// 清理音频资源
void CleanupAudioPlayer(MusicPlayerState& state) {
    state.running = false;
    
    if (state.nextCheckThread.joinable()) state.nextCheckThread.join();
    if (state.playThread.joinable()) state.playThread.join();
    
    std::lock_guard<std::mutex> lock(state.audioMutex);
    
    if (state.initialized) {
        if (state.player && state.device) {
            state.player->Exit(state.device->GetDevice(), state.decoder->GetDecoder());
        }
        
        delete state.pather;
        delete state.decoder;
        delete state.player;
        delete state.timer;
        delete state.buffer;
        
        state.initialized = false;
    }
}

int main(int argc, char** argv) {
    // 初始化GLFW
    if (!glfwInit()) return 1;

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "BeePlayer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 启用垂直同步

    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

	// 字体加载 - 添加中文字符范围
	ImFont* font = nullptr;
	try {
		// 定义中文字符范围
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
		font = io.Fonts->AddFontFromFileTTF("hwkt.ttf", 23.0f, &config, ranges);

		if (font) {
			io.FontDefault = font;
			LOG_INFO("ImGUI -> Font Loaded!");

			// 重建字体纹理
			ImGui_ImplOpenGL3_DestroyFontsTexture();
			ImGui_ImplOpenGL3_CreateFontsTexture();
		} else {
			// 使用默认字体（不支持中文）
			io.Fonts->AddFontDefault();
			LOG_WARNING("ImGUI -> Can not load the font, using default!");
		}
	} catch (const std::exception& e) {
		std::cerr << "ImGUI -> 字体加载错误: " << e.what() << std::endl;
		// 回退到默认字体
		io.Fonts->AddFontDefault();
	}

    // 创建播放器状态
    MusicPlayerState playerState;
    
    // 获取音乐路径
    std::string rootPath;
    if (argc == 3 && std::string(argv[1]) == "-root") {
        rootPath = argv[2];
    } else {
        // 在图形界面中要求用户选择目录
        bool pathSelected = false;
        
        // 主循环
        while (!glfwWindowShouldClose(window) && !pathSelected) {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(io.DisplaySize);
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
                pathSelected = true; // 为了退出循环
            }
            
            ImGui::End();

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }
    
    // 初始化音频播放器
    if (!rootPath.empty() && !InitializeAudioPlayer(playerState, rootPath)) {
        LOG_ERROR("ImGUI -> Can not Init the Audio Player!");
        return 1;
    }
    
    // 更新进度条的函数
    auto updateProgress = [&] {
        if (playerState.initialized && playerState.timer) {
        	double current = playerState.buffer->GetGlobalFrameCount() / playerState.decoder->GetDecoder().outputSampleRate;
        	double total = playerState.timer->GetTotalFrames() / playerState.decoder->GetDecoder().outputSampleRate;
            
            if (total > 0) {
                playerState.progress = current / total;
            } else {
                playerState.progress = 0.0f;
            }
        }
    };

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // 开始ImGui帧
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
        updateProgress();
        ImGui::Text("进度");
        ImGui::ProgressBar(playerState.progress, ImVec2(-1, 25));
        
        // 控制按钮区域
        ImGui::BeginGroup();
        if (ImGui::Button("上一曲", ImVec2(80, 40))) { 
            if (playerState.initialized) {
                std::lock_guard<std::mutex> lock(playerState.audioMutex);
                playerState.player->Switch(
                    *playerState.pather, *playerState.decoder, 
                    *playerState.device, data_callback, *playerState.timer, 
                    *playerState.buffer, SwitchAction::PREV
                );
                playerState.currentTrack = playerState.pather->Index();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(playerState.isPlaying ? "暂停" : "播放", ImVec2(80, 40))) { 
            if (playerState.initialized) {
                std::lock_guard<std::mutex> lock(playerState.audioMutex);
                if (ma_device_is_started(&playerState.device->GetDevice())) {
                    ma_device_stop(&playerState.device->GetDevice());
                    playerState.isPlaying = false;
                } else {
                    ma_device_start(&playerState.device->GetDevice());
                    playerState.isPlaying = true;
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("停止", ImVec2(80, 40))) { 
            if (playerState.initialized) {
                std::lock_guard<std::mutex> lock(playerState.audioMutex);
                ma_device_stop(&playerState.device->GetDevice());
                playerState.timer->ResetStatus();
                playerState.progress = 0.0f;
                playerState.isPlaying = false;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("下一曲", ImVec2(80, 40))) { 
            if (playerState.initialized) {
                std::lock_guard<std::mutex> lock(playerState.audioMutex);
                playerState.player->Switch(
                    *playerState.pather, *playerState.decoder, 
                    *playerState.device, data_callback, *playerState.timer, 
                    *playerState.buffer, SwitchAction::NEXT
                );
                playerState.currentTrack = playerState.pather->Index();
            }
        }
        ImGui::EndGroup();

        // 音量控制
        ImGui::Separator();
        ImGui::Text("音量控制");
        if (ImGui::SliderFloat("##volume", &playerState.volume, 0.0f, 1.0f, "%.1f")) {
            if (playerState.initialized) {
                // 在实际应用中设置音量
                // ma_device_set_master_volume(&playerState.device->GetDevice(), playerState.volume);
            }
        }
        
        // 播放列表
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
    			size_t currentIndex = playerState.pather->Index();

    			// 设置新索引
    			playerState.pather->SetIndex(playerState.currentTrack);

    			// 如果新索引大于当前索引，使用 NEXT 动作
    			// 如果小于，使用 PREV 动作
    			SwitchAction action = (playerState.currentTrack > currentIndex)
									 ? SwitchAction::NEXT : SwitchAction::PREV;

    			// 调用切换函数
    			playerState.player->Switch(
					*playerState.pather, *playerState.decoder,
					*playerState.device, data_callback, *playerState.timer,
					*playerState.buffer, action
				);

    			playerState.isPlaying = true;
    		}
    	}

        ImGui::End();

        // 渲染
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 清理音频资源
    CleanupAudioPlayer(playerState);
    
    // 清理ImGui和GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}