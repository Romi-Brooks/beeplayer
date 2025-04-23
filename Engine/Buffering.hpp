//
// Created by Romi on 25-4-23.
//

#ifndef BUFFERING_HPP
#define BUFFERING_HPP


#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "../miniaudio/miniaudio.h"

class AudioBuffering {
public:
    struct Buffer {
        std::vector<ma_uint8> s_data;     // PCM数据存储
        ma_uint64 s_startFrame = 0;       // 起始帧位置
        ma_uint64 s_totalFrames = 0;      // 总帧数
        std::atomic<bool> ready{false}; // 就绪状态
    };

    explicit AudioBuffering(ma_decoder* decoder) : p_keepFilling(true) {
        p_bufferFillerThread = std::thread(&AudioBuffering::BufferFiller, this, decoder);
    }

    ~AudioBuffering() {
        p_keepFilling.store(false);
        if (p_bufferFillerThread.joinable()) {
            p_bufferFillerThread.join();
        }
    }

    // 获取当前活动缓冲区（线程安全）
    Buffer& GetActiveBuffer() {
        return p_buffers[p_activeBuffer.load()];
    }

    // 切换缓冲区（线程安全）
    void SwitchBuffer() {
        p_activeBuffer.store((p_activeBuffer.load() + 1) % 2);
    }

    // 获取全局帧计数器
    ma_uint64 GetGlobalFrameCount() const {
        return p_globalFrameCount.load();
    }

	void AddGlobalFrames(ma_uint64 frames) {
    	p_globalFrameCount.fetch_add(frames, std::memory_order_relaxed);
    }
	auto& INTF_GetActiveBuffer() {
    	return p_activeBuffer;
    }
private:
    void BufferFiller(ma_decoder* decoder) {
        int nextBuffer = 0;
        while (p_keepFilling) {
            // 等待目标缓冲区消耗完毕
            if (p_buffers[nextBuffer].ready) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            // 计算目标帧数（500ms缓冲）
            const ma_uint32 targetFrames = p_outputSampleRate / 2;
            p_buffers[nextBuffer].s_data.resize(targetFrames * ma_get_bytes_per_frame(decoder->outputFormat, decoder->outputChannels));

        	// 读取音频数据
            ma_uint64 framesRead;
            ma_result result = ma_decoder_read_pcm_frames(
                decoder,
                p_buffers[nextBuffer].s_data.data(),
                targetFrames,
                &framesRead
            );

            if (result == MA_SUCCESS && framesRead > 0) {
                // 更新缓冲区元数据
                p_buffers[nextBuffer].s_startFrame = p_globalFrameCount;
                p_buffers[nextBuffer].s_totalFrames = framesRead;
                p_buffers[nextBuffer].ready = true;

                // 准备填充下一个缓冲区
                nextBuffer = (nextBuffer + 1) % 2;
                p_globalFrameCount += framesRead;
            } else {
                // 处理解码完成或错误
                p_keepFilling.store(false);
            }
        }
    }

    Buffer p_buffers[2];                   // 双缓冲区
    std::atomic<int> p_activeBuffer{0};    // 当前活动缓冲区索引
    std::atomic<ma_uint64> p_globalFrameCount{0}; // 全局帧计数器
    ma_uint32 p_outputSampleRate = 0;          // 输出采样率
    std::thread p_bufferFillerThread;      // 数据填充线程
    std::atomic<bool> p_keepFilling;       // 线程运行标志
};


#endif //BUFFERING_HPP
