/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *
 *  Beeplayer non-Release Version
 *  A minimal cross-platform music player based on miniaudio and C++.
 *
 *  Thanks to David Reid provided us a such powerful and useful lib "miniaudio"
 *
 *  All of the code is not the final version now,
 *  and it should be logically rewritten once I have implemented the basic functions.
 *
 *  Thanks to music make the world so beautiful. :)
 */

#include "miniaudio/miniaudio.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>
#include <chrono>
#include <cstring>
namespace fs = std::filesystem;

// All of This fucking code makes all the huge mess, what I wish I could quickly rewite it as a class file. :(


// To provide the Listen Event can be Non-blocking.
#ifdef _WIN32 // For windows platfrom Non-blocking input
#include <conio.h>
bool IsInputAvailable() {
    return _kbhit() != 0;
}
#else
#include <sys/select.h>
bool IsInputAvailable() {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    timeval timeout = {0, 0};
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout) > 0;
}
#endif

// Forward Functions
void ListenEvent(ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device);
std::vector<std::string> PathGenner(const std::string& root);
void ProgressThread(ma_decoder* pDecoder);
int CleanRes(ma_device& device, ma_decoder& decoder);

// Global Setting
int GlobalCounter = 0;
std::string rootPath = "E:/Music";
auto SongName = PathGenner(rootPath);
std::atomic<ma_uint64> globalTotalFrames{0};


// Pather
std::vector<std::string> PathGenner(const std::string& root) {
    std::vector<std::string> SongName;
    const fs::path root_path(root); // give iterator a root path

    // Pre-check
    if(!fs::exists(root_path)) return SongName;
    if(!fs::is_directory(root_path)) return SongName;

    // iterator:
    try {
        auto iterator_opt = fs::directory_options::skip_permission_denied; // skip the folder which is permission_denied
        auto iterator_rec = fs::recursive_directory_iterator(root_path,iterator_opt); // run iterator with iterator_opt

        for(const auto& file : iterator_rec) {
            try {
                if(!file.is_regular_file()) continue;

                std::string extension = file.path().extension().string(); // get file's extension
                // if "Wav" or something -> "wav"
                std::ranges::transform(extension.begin(), extension.end(), extension.begin(),
                    [](unsigned char c) { return std::tolower(c);}); // lambda fuc: trans the upper charc to lower

                if(extension == ".mp3" || extension == ".wav") {
                    SongName.push_back(file.path().filename().string());
                }
            } catch (...) {
                // run
            }
        }
    } catch (...) {
        // run
    }
    return SongName;
}

std::string NextFile() {
    if (GlobalCounter == (SongName.size()-1)) return rootPath + "/" + *SongName.end();
    GlobalCounter = (GlobalCounter + 1) % SongName.size();
    return rootPath + "/" + SongName[GlobalCounter];
}

std::string PreFile() {
    if (GlobalCounter == 0) return rootPath + "/" + *SongName.begin();
    GlobalCounter = (GlobalCounter - 1 + SongName.size()) % SongName.size();
    return rootPath + "/" + SongName[GlobalCounter];
}

std::string PathInit() {
    return rootPath + "/" + SongName[GlobalCounter];
}

std::string FileName(const std::string& path) {
    return fs::path(path).filename().string();
}

// 新增全局结构体和变量
struct AudioBuffer {
    std::vector<ma_uint8> data;    // 原始音频数据
    ma_uint64 startFrame = 0;      // 缓冲区起始帧位置
    ma_uint64 totalFrames = 0;     // 缓冲区总帧数
    std::atomic<bool> ready{false};// 缓冲区就绪状态
};

AudioBuffer buffers[2];            // 双缓冲数组
std::atomic<int> activeBuffer{0};  // 当前活动缓冲区索引
std::atomic<ma_uint64> globalFrameCount{0}; // 全局已播放帧数
ma_uint32 outputSampleRate = 0;    // 采样率（需初始化时获取）
std::thread bufferFillerThread;    // 缓冲填充线程
std::atomic<bool> keepFilling{true}; // 线程控制标志

// 缓冲填充函数
void BufferFiller(ma_decoder* pDecoder) {
    int nextBuffer = 0;
    while (keepFilling) {
        // 等待当前缓冲区消耗过半再填充
        if (buffers[nextBuffer].ready) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // 计算需要读取的帧数（示例：500ms的缓冲）
        const ma_uint32 targetFrames = outputSampleRate * 0.5;
        buffers[nextBuffer].data.resize(targetFrames * ma_get_bytes_per_frame(pDecoder->outputFormat, pDecoder->outputChannels));

        // 读取音频数据
        ma_uint64 framesRead;
        ma_result result = ma_decoder_read_pcm_frames(
            pDecoder,
            buffers[nextBuffer].data.data(),
            targetFrames,
            &framesRead
        );

        if (result == MA_SUCCESS && framesRead > 0) {
            buffers[nextBuffer].startFrame = globalFrameCount;
            buffers[nextBuffer].totalFrames = framesRead;
            buffers[nextBuffer].ready = true;
            nextBuffer = (nextBuffer + 1) % 2;
        } else {
            // 处理文件结束或错误
            break;
        }
    }
}

void ProgressThread(ma_decoder* pDecoder) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        {
            const auto current = globalFrameCount.load();
            const auto total = globalTotalFrames.load();

            const double currentTime = current / outputSampleRate;
            const double totalTime = total / outputSampleRate;

            std::cout << "\rNow Playing:"
                      << std::setfill('0') << std::setw(2) << static_cast<int>(currentTime) / 60 << ":"
                      << std::setfill('0') << std::setw(2) << static_cast<int>(currentTime) % 60 << "/"
                      << std::setfill('0') << std::setw(2) << static_cast<int>(totalTime) / 60 << ":"
                      << std::setfill('0') << std::setw(2) << static_cast<int>(totalTime) % 60
                      << "  \r" << std::flush; // 仅刷新，不换行
        }
    }
}

// miniaudio Callback function
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    static ma_uint64 consumedFrames = 0; // 当前缓冲区已消耗帧数
    const int currentBufIdx = activeBuffer.load();
    AudioBuffer& currentBuf = buffers[currentBufIdx];

    if (!currentBuf.ready) {
        memset(pOutput, 0, frameCount * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels));
        return;
    }

    // 计算可复制帧数
    const ma_uint64 availableFrames = currentBuf.totalFrames - consumedFrames;
    const ma_uint64 framesToCopy = std::min(static_cast<ma_uint64>(frameCount), availableFrames);

    // 复制数据到输出缓冲区
    const size_t bytesToCopy = framesToCopy * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
    memcpy(
        pOutput,
        currentBuf.data.data() + consumedFrames * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels),
        bytesToCopy
    );

    // 更新计数器
    consumedFrames += framesToCopy;
    globalFrameCount += framesToCopy;

    // 处理缓冲区切换
    if (consumedFrames >= currentBuf.totalFrames) {
        currentBuf.ready = false;
        activeBuffer.store((currentBufIdx + 1) % 2);
        consumedFrames = 0;
    }

    // 填充剩余空间（如果有）
    if (framesToCopy < frameCount) {
        const size_t remainingBytes = (frameCount - framesToCopy) * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
        memset(static_cast<char *>(pOutput) + bytesToCopy, 0, remainingBytes);
    }
}

// miniaudio Init Functions
int InitDecoder(const std::string& path, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    ma_result result = ma_decoder_init_file(path.c_str(), nullptr, &decoder);
    if (result != MA_SUCCESS) {
        std::cout << "Error to loading the file:" << path << std::endl;
        return -2;
    }
    ma_uint64 TotalFrames = 0;
    // 获取采样率
    outputSampleRate = decoder.outputSampleRate;
    // 获取帧长度
    ma_data_source_get_length_in_pcm_frames(&decoder, &TotalFrames);
    globalTotalFrames.store(TotalFrames);
    // std::cout << "Init Decoder Completed." << std::endl;
    return 1;
}

void InitConfig(ma_device_config& deviceConfig, ma_decoder& decoder) {
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;
    // std::cout << "Init Player Config Completed." << std::endl;
}

int InitDevice(ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
        std::cout << "Error to init device." << std::endl;
        ma_decoder_uninit(&decoder);
        return -3;
    }
    // std::cout << "Init Device Completed." << std::endl;
    return 1;
}

int PlayFile(ma_device& device, ma_decoder& decoder) {
    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "Failed to start playback device." << std::endl;
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return  -4;
    }
    return 1;
}

// Player Init Functions
void InitEngine(const std::string& path, ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    InitDecoder(path, decoder, deviceConfig, device);
    InitConfig(deviceConfig, decoder);
    InitDevice(decoder, deviceConfig, device);
    std::cout << "Now playing:" << FileName(SongName[GlobalCounter]) << std::endl;
    PlayFile(device, decoder);

    // 启动缓冲填充线程
    keepFilling = true;
    bufferFillerThread = std::thread(BufferFiller, &decoder);

    // 启动进度线程
    std::thread(ProgressThread, &decoder).detach();
    ListenEvent(decoder, deviceConfig, device);
}

void InitEngineWithoutDevice(const std::string& path, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    InitDecoder(path, decoder, deviceConfig, device);
    InitConfig(deviceConfig, decoder);

    std::cout << "Now playing:" << FileName(SongName[GlobalCounter]) << std::endl;
    PlayFile(device, decoder);

    // Rerun the double buffering progress
    bufferFillerThread = std::thread(BufferFiller, &decoder);
    ListenEvent(decoder, deviceConfig, device);
}

void SwitchMusic(ma_decoder& decoder) {
    // 停止填充线程
    keepFilling = false;
    if (bufferFillerThread.joinable()) {
        bufferFillerThread.join();
    }

    // 重置播放计数器
    globalFrameCount.store(0);
    keepFilling = true;

    // 重置缓冲状态
    buffers[0].ready = false;
    buffers[1].ready = false;
    activeBuffer = 0;
    globalFrameCount = 0;
}

// User Input
void ListenEvent(ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    while (true) {
        // 非阻塞检查用户输入
        if (IsInputAvailable()) {
            char key;
            std::cin >> key;
            switch (key) {
                case 'e': {
                    CleanRes(device, decoder);
                    exit(0);
                }
                case 'p': {
                    if (ma_device_is_started(&device)) {
                        ma_device_stop(&device);
                        std::cout << "Paused." << std::endl;
                    } else {
                        ma_device_start(&device);
                        std::cout << "Resumed." << std::endl;
                    }
                    break;
                }
                case 'n': {
                    SwitchMusic(decoder);
                    InitEngineWithoutDevice(NextFile(), decoder, deviceConfig, device);
                    break;
                }
                case 'u': {
                    SwitchMusic(decoder);
                    InitEngineWithoutDevice(PreFile(), decoder, deviceConfig, device);
                    break;
                }
                default: {
                    std::cout << "Unknown Command." << std::endl;
                    break;
                }
            }
        }

        // PlayDone Switch
        ma_uint64 current = globalFrameCount.load();
        ma_uint64 total = globalTotalFrames.load();
        if (total > 0 && current >= total) {
            std::cout << "\nEnd of track, switching to next..." << std::endl;
            SwitchMusic(decoder);
            InitEngineWithoutDevice(NextFile(), decoder, deviceConfig, device);
            // 重置计数器避免重复触发
            current = 0;
            total = globalTotalFrames.load();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int CleanRes(ma_device& device, ma_decoder& decoder) {
    keepFilling = false;
    if (bufferFillerThread.joinable()) {
        bufferFillerThread.join();
    }
    // 清理资源
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    return 1;
}

int main() {
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;
    InitEngine(PathInit(), result, decoder, deviceConfig, device);

    CleanRes(device,decoder);
}