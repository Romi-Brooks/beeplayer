/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Controller.cpp
 *  Lib: Beeplayer Core engine Provided the Public Wrapper interface
 *  Author: Romi Brooks
 *  Date: 2025-07-03
 *  Type: Controller, Core Engine, Wrapper, Interface
 */

#include "Controller.hpp"
#include "../Engine/DataCallback.hpp"
#include "../Log/LogSystem.hpp"

PlayerController::PlayerController() {
    // 获取设备单例
    Device = &AudioDevice::GetDeviceInstance();
}

PlayerController::~PlayerController() {
    Cleanup();
}

bool PlayerController::Initialize(const std::string& rootPath) {
    std::lock_guard<std::mutex> lock(audioMutex);

    try {
        // 创建路径对象
        Pather = std::make_unique<Path>(rootPath);

        // 获取媒体文件列表
        tracks = Pather->GetFiles();
        if (tracks.empty()) {
            Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_CONTROLLER, "No media files found!");
            return false;
        }

        // 初始化音频组件
        if (!InitializeAudioComponents()) {
            return false;
        }

        // 启动监控线程
        running = true;
        nextCheckThread = std::thread(&PlayerController::NextFileCheckThread, this);

        initialized = true;
        return true;
    } catch (const std::exception& e) {
        Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_CONTROLLER,
                   "Initialization error: " + std::string(e.what()));
        return false;
    }
}

bool PlayerController::InitializeAudioComponents() {
    try {
        // 创建解码器
        Decoder = std::make_unique<AudioDecoder>();

        // 创建播放器
        Player = std::make_unique<AudioPlayer>();

        // 初始化解码器（使用第一个文件）
        Player->InitDecoder(*Pather, *Decoder);

        // 创建状态计时器和缓冲区
        Timer = std::make_unique<Status>(*Decoder);
        Buffer = std::make_unique<AudioBuffering>(&Decoder->GetDecoder());

        // 初始化设备
        Player->InitDevice(*Decoder, *Device, data_callback, *Buffer);

        return true;
    } catch (const std::exception& e) {
        Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_CONTROLLER,
                   "Audio component initialization failed: " + std::string(e.what()));
        return false;
    }
}

void PlayerController::Cleanup() {
    running = false;
    if (nextCheckThread.joinable()) nextCheckThread.join();
    if (playThread.joinable()) playThread.join();

    std::lock_guard<std::mutex> lock(audioMutex);

    if (initialized) {
        if (Player && Device && Decoder) {
            Player->Exit(*Device, *Decoder);
        }

        // 释放资源（智能指针会自动管理）
        Pather.reset();
        Decoder.reset();
        Player.reset();
        Timer.reset();
        Buffer.reset();

        initialized = false;
    }
}

void PlayerController::NextFileCheckThread() {
	while (running) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		if (initialized) {

			// 检查歌曲是否结束
			if (Timer && Decoder && Buffer) {
				const auto currentTime = Buffer->GetGlobalFrameCount() / Decoder->GetDecoder().outputSampleRate;
				const auto totalTime = Timer->GetTotalFrames() / Decoder->GetDecoder().outputSampleRate;

				if (totalTime > 0 && currentTime >= totalTime) {
					Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_CONTROLLER, "End of track, switching to next...");
                                Next();
				}
			}

			// 重置跳转状态（如果超过500ms）
			auto now = std::chrono::steady_clock::now();
			if (isSeeking.load() && (now - lastSeekTime) > std::chrono::milliseconds(500)) {
				isSeeking.store(false);
			}
		}
	}
}

void PlayerController::Play() {
    std::lock_guard<std::mutex> lock(audioMutex);
    if (initialized && Player && Device && !isPlaying) {
        Player->Play(*Device, *Decoder, *Timer, *Buffer);
        isPlaying = true;
    }
}

void PlayerController::Pause() {
    std::lock_guard<std::mutex> lock(audioMutex);
    if (initialized && Player && Device) {
        Player->Pause(*Device);
        isPlaying = false;
    }
}

void PlayerController::Stop() {
	std::lock_guard<std::mutex> lock(audioMutex);

	if (initialized && Player && Device) {
            {
                // pause the play 1st
                Player->Pause(*Device);
                // 设置跳转状态
                isSeeking.store(true);
                lastSeekTime = std::chrono::steady_clock::now();

                // Seek to this position
                {
                    if (initialized && Player && Decoder) {
                        // 执行跳转
                        Player->Seek(*Decoder, static_cast<ma_uint64>(0));
                    }
                }

                // Buffer Proc
                {
                    ma_uint64 ori_Frame = Buffer->GetGlobalFrameCount(); // For Log Using
                    Buffer->SetGlobalFrameCount(static_cast<ma_uint64>(0));
                    Buffer->CleaerBuffer(); // make sure there're no any data in the buffer block
                    Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_BUFFERING, "Set Read Frame form ", ori_Frame, " To ", Buffer->GetGlobalFrameCount());
                }
            }

            isPlaying = false;
	}
}

void PlayerController::Switch(const size_t Index) {
	std::lock_guard<std::mutex> lock(audioMutex);

	if (!initialized || tracks.empty()) return;


    if (Index >= Pather->TotalSong()) {
		Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_CONTROLLER, "Error to Switch the song, out of index range.");
		return;
	}
    Pather->SetIndex(Index+1);

	if (isPlaying) {
		Stop();
	}
	// 切换到Index
	Player->Switch(*Pather, *Decoder, *Device, data_callback, *Timer, *Buffer,
				  SwitchAction::PREV);

        // 更新当前曲目索引
        currentTrack = Index;

        // 触发回调通知UI
        if (trackChangeCallback) {
            trackChangeCallback(Index);
        }

	// 重置播放状态
	isPlaying = false;
    // progress = 0.0f;
        Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_CONTROLLER,
                         "Switched to track: " + std::to_string(Index) +
                         ", Path: " + Pather->CurrentFilePath());
}

void PlayerController::Next() {
    std::lock_guard<std::mutex> lock(audioMutex);

    if (!initialized || tracks.empty()) return;

    // 计算下一首索引
    size_t next = (currentTrack + 1) % tracks.size();

    // 切换到下一首
    Player->Switch(*Pather, *Decoder, *Device, data_callback, *Timer, *Buffer,
                  SwitchAction::NEXT);

    // 更新当前曲目
    currentTrack = next;

    // 通知状态改变
    if (trackChangeCallback) {
        trackChangeCallback(next);
    }
}

void PlayerController::Prev() {
    std::lock_guard<std::mutex> lock(audioMutex);

    if (!initialized || tracks.empty()) return;

    // 计算上一首索引
    size_t prev = (currentTrack == 0) ? tracks.size() - 1 : currentTrack - 1;

    // 切换到上一首
    Player->Switch(*Pather, *Decoder, *Device, data_callback, *Timer, *Buffer,
                  SwitchAction::PREV);

    // 更新当前曲目
    currentTrack = prev;

    // 通知状态改变
    // 触发回调通知UI
    if (trackChangeCallback) {
        trackChangeCallback(prev);
    }
}

void PlayerController::SeekToPosition(const float progress) {
	// 设置跳转状态
	isSeeking.store(true);
	lastSeekTime = std::chrono::steady_clock::now();

    // Get the seek information
    const auto totalFrame = Timer->GetTotalFrames();
    const auto seekFrame = progress * static_cast<float>(totalFrame);

    // Seek to this position
    {
        std::lock_guard<std::mutex> lock(audioMutex);
        if (initialized && Player && Decoder) {
            // 执行跳转
            Player->Seek(*Decoder, static_cast<ma_uint64>(seekFrame));
        }
    }

    {
        ma_uint64 ori_Frame = Buffer->GetGlobalFrameCount(); // For Log Using

        Buffer->SetGlobalFrameCount(static_cast<ma_uint64>(seekFrame));
        Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_BUFFERING, "Set Read Frame form ", ori_Frame, " To ", Buffer->GetGlobalFrameCount());
    }
}

// void PlayerController::UpdateProgress() {
// 	std::lock_guard<std::mutex> lock(progressMutex);

// 	if (!initialized || !Decoder || !Timer || !Buffer) return;

// 	// 如果最近跳转过，跳过更新
// 	auto now = std::chrono::steady_clock::now();
// 	if (isSeeking.load() && (now - lastSeekTime) < std::chrono::milliseconds(500)) {
// 		return;
// 	}

// 	// 获取当前播放位置
// 	const float currentTime = static_cast<float>(Buffer->GetGlobalFrameCount()) / static_cast<float>(Decoder->GetDecoder().outputSampleRate);
// 	const float totalTime = static_cast<float>(Timer->GetTotalFrames()) / static_cast<float>(Decoder->GetDecoder().outputSampleRate);

// 	// 更新进度
// 	if (totalTime > 0) {
// 		progress.store(currentTime / totalTime);
// 	} else {
// 		progress.store(0.0f);
// 	}
// }

void PlayerController::SetVolume(float vol) {
    std::lock_guard<std::mutex> lock(audioMutex);

    volume = std::clamp(vol, 0.0f, 1.0f);

    if (initialized && Device) {
        ma_device_set_master_volume(&Device->GetDevice(), volume);
    }
}

const std::string& PlayerController::GetCurrentTrackName() const {
    static const std::string empty = "No track";
    if (tracks.empty() || currentTrack >= tracks.size()) {
        return empty;
    }
    return tracks[currentTrack];
}

const std::string PlayerController::GetCurrentTrackProducer() const {
    static const std::string empty = "";

    // 检查tracks和currentTrack的有效性
    if (tracks.empty() || currentTrack >= tracks.size()) {
        return empty;
    }

    // 获取当前文件路径
    const std::string& filePath = Pather->CurrentFilePath();
    if (filePath.empty()) {
        return empty;
    }

    // 获取音频元数据读取器实例
    auto& reader = AudioMetadataReader::getInstance();

    bool isPureAscii = Encoder->IsPureAscii(Pather->CurrentFilePath());
    if  (isPureAscii == false) {
        // 直接返回结果，确保getSongProducer()返回有效引用
        return reader.getSongProducer(Encoder->u8tou16(Pather->CurrentFilePath()));
    } else{
        return reader.getSongProducer(Pather->CurrentFilePath());
    }
}

const std::vector<unsigned char> PlayerController::GetCurrentTrackAlbum() {
    // 返回空向量表示无数据
    static const std::vector<unsigned char> empty;

    // 检查tracks和currentTrack的有效性
    if (tracks.empty() || currentTrack >= tracks.size()) {
        return empty;
    }

    // 获取当前文件路径
    const std::string& filePath = Pather->CurrentFilePath();
    if (filePath.empty()) {
        return empty;
    }

    // 获取音频元数据读取器实例
    auto& reader = AudioMetadataReader::getInstance();

    try {
        bool isPureAscii = Encoder->IsPureAscii(filePath);
        if (!isPureAscii) {
            return reader.getAlbumCover(Encoder->u8tou16(filePath));
        } else {
            return reader.getAlbumCover(filePath);
        }
    } catch (const std::exception& e) {
        // 记录错误但不中断程序
        Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_METADATA, "Failed to get album art: " , e.what());
        return empty;
    }
}

void PlayerController::NotifyTrackChanged() {
    if (trackChangeCallback) {
        trackChangeCallback(currentTrack);
    }
}




float PlayerController::GetCurrentProgress() const {
    if (!initialized || !Timer || Timer->GetTotalFrames() == 0)
        return 0.0f;

    return static_cast<float>(GetCurrentFrame()) /
           static_cast<float>(GetTotalFrames());
}

float PlayerController::GetCurrentTime() const {
    if (!initialized || !Timer)
        return 0.0f;


    return static_cast<float>(GetCurrentFrame() / Decoder->GetDecoder().outputSampleRate);
}

float PlayerController::GetTotalTime() const {
    if (!initialized || !Timer || !Timer->GetTotalFrames())
        return 0.0f;

    return static_cast<float>(GetTotalFrames()) / Decoder->GetDecoder().outputSampleRate;
}

ma_uint64 PlayerController::GetCurrentFrame() const {
    return Buffer ? static_cast<float>(Buffer->GetGlobalFrameCount()) : 0;
}

ma_uint64 PlayerController::GetTotalFrames() const {
    return Buffer ? static_cast<float>(Timer->GetTotalFrames()) : 0;
}
