#ifndef MUSICPLAYERSTATE_HPP
#define MUSICPLAYERSTATE_HPP

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

class PlayerController {
public:
    // 回调类型定义
    using StateChangeCallback = std::function<void()>;
    
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
    void UpdateProgress();

    // 状态获取
    float GetProgress() const { return progress.load(); }
    float GetVolume() const { return volume; }
    void SetVolume(float vol);
    bool IsPlaying() const { return isPlaying.load(); }
    bool IsInitialized() const { return initialized; }
    size_t GetCurrentTrackIndex() const { return currentTrack.load(); }
    const std::string& GetCurrentTrack() const;
    const std::vector<std::string>& GetTracks() const { return tracks; }

    // 回调设置
    void SetStateChangeCallback(StateChangeCallback callback) {
        stateChangeCallback = callback;
    }
    
    // 内部使用的回调（由AudioPlayer调用）
    void NotifyTrackChanged();

private:
    // 内部初始化方法
    bool InitializeAudioComponents();
    
    // 线程函数
    void NextFileCheckThread();
    
    // 成员变量
    std::vector<std::string> tracks;
    std::atomic<size_t> currentTrack{0};
    std::atomic<bool> isPlaying{false};
    std::atomic<bool> AutoSwitch{false};
    std::atomic<float> progress{0.0f};
    std::atomic<bool> isSeeking{false};
    std::chrono::steady_clock::time_point lastSeekTime;
    mutable std::mutex progressMutex; // 独立的进度互斥锁
    float volume = 0.8f;
    
    // 全局对象（使用智能指针管理）
    std::unique_ptr<Path> Pather;
    std::unique_ptr<AudioDecoder> Decoder;
    AudioDevice* Device = nullptr;
    std::unique_ptr<AudioPlayer> Player;
    std::unique_ptr<Status> Timer;
    std::unique_ptr<AudioBuffering> Buffer;
    
    // 线程控制
    std::atomic<bool> running{true};
    std::thread nextCheckThread;
    std::thread playThread;
    
    // 状态标志
    bool initialized = false;
    
    // 回调函数
    StateChangeCallback stateChangeCallback;
    
    // 互斥锁
    mutable std::mutex audioMutex;
};

#endif // MUSICPLAYERSTATE_HPP