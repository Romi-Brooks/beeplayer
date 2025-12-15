/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Controller.hpp
 *  Lib: Beeplayer Core engine Provided the Public Wrapper interface definitions
 *  Author: Romi Brooks
 *  Date: 2025-07-03
 *  Type: Controller, Core Engine, Wrapper, Interface
 */
#ifndef MUSICPLAYERSTATE_HPP
#define MUSICPLAYERSTATE_HPP

// Standard Lib
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include <functional>
#include <memory>
#include <chrono>

// Basic Lib
#include "../Engine/Buffering.hpp"
#include "../Engine/Decoder.hpp"
#include "../Engine/Device.hpp"
#include "../Engine/Player.hpp"
#include "../Engine/Status.hpp"
#include "../FileSystem/Path.hpp"
#include "../FileSystem/Metadata.hpp"
#include "../FileSystem/Encoding.hpp"

class PlayerController {
public:
    // 回调类型定义
    using TrackChangeCallback = std::function<void(size_t newIndex)>;
    
    PlayerController();
    ~PlayerController();
    
    // 禁止拷贝和赋值
    PlayerController(const PlayerController&) = delete;
    PlayerController& operator=(const PlayerController&) = delete;
    
    // 初始化音频播放器
    bool Initialize(const std::string& rootPath);
    
    // 清理资源
    void Cleanup();
    
    // 播放控制
    void Play();
    void Pause();
    void Stop();
    void Next();
    void Prev();
    void Switch(size_t Index);
    void SeekToPosition(const float Progress);
    void SetVolume(float vol);

    // Progress
    float GetCurrentProgress() const;
    float GetCurrentTime() const; // 当前播放时间(秒)
    float GetTotalTime() const;   // 总时长(秒)

    // 状态获取
    float GetVolume() const { return volume; }

    bool IsPlaying() const { return isPlaying.load(); }
    bool IsInitialized() const { return initialized; }
    size_t GetCurrentTrackIndex() const { return currentTrack.load(); }
    const std::string& GetCurrentTrackName() const;
    const std::string GetCurrentTrackProducer() const;
    const std::vector<unsigned char> GetCurrentTrackAlbum();
    const std::vector<std::string>& GetTracks() const { return tracks; }

    // 回调设置
    void SetTrackChangeCallback(TrackChangeCallback callback) {
        trackChangeCallback = callback;
    }
    
    // 内部使用的回调（由AudioPlayer调用）
    void NotifyTrackChanged();

private:
    // 内部初始化方法
    bool InitializeAudioComponents();
    
    // 线程函数
    void NextFileCheckThread();
    
    // 成员变量
    std::vector<std::string> tracks; // tacks name
    std::atomic<size_t> currentTrack{0};
    std::atomic<bool> isPlaying{false};
    std::atomic<bool> AutoSwitch{false};
    std::atomic<bool> isSeeking{false};
    std::chrono::steady_clock::time_point lastSeekTime;
    float volume = 0.8f;
    
    // 全局对象（使用智能指针管理）
    std::unique_ptr<Path> Pather;
    std::unique_ptr<Encoding> Encoder;
    std::unique_ptr<AudioDecoder> Decoder;
    AudioDevice* Device = nullptr;
    std::unique_ptr<AudioPlayer> Player;
    std::unique_ptr<Status> Timer;
    std::unique_ptr<AudioBuffering> Buffer;
    std::unique_ptr<AudioMetadataReader> Metadata;
    
    // 线程控制
    std::atomic<bool> running{true};
    std::thread nextCheckThread;
    std::thread playThread;
    
    // 状态标志
    bool initialized = false;
    
    // 回调函数
    TrackChangeCallback trackChangeCallback; // 新增回调
    
    // 互斥锁
    mutable std::mutex audioMutex;

    // Progress
    ma_uint64 GetCurrentFrame() const;
    ma_uint64 GetTotalFrames() const;
};

#endif // MUSICPLAYERSTATE_HPP
