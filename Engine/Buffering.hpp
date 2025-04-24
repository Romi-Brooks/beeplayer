/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Buffering.hpp
 *  Lib: Beeplayer Core engine Audio Buffer -> Double buffering lib
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: Core Engine
 */

#ifndef BUFFERING_HPP
#define BUFFERING_HPP

#include <atomic>
#include <thread>
#include <vector>

#include "../miniaudio/miniaudio.h"

class AudioBuffering {
public:
	struct Buffer {
		std::vector<ma_uint8> s_data;    // 原始音频数据
		ma_uint64 s_startFrame = 0;      // 缓冲区起始帧位置
		ma_uint64 s_totalFrames = 0;     // 缓冲区总帧数
		std::atomic<bool> s_ready{false};// 缓冲区就绪状态
	};

    explicit AudioBuffering(ma_decoder *decoder);

    ~AudioBuffering();
	Buffer* GetBuffers() { return p_buffers; }
	int GetActiveBuffer() const { return p_activeBuffer.load(); }
	ma_uint64 GetGlobalFrameCount() const { return p_globalFrameCount.load(); }
	ma_uint32 GetOutputSampleRate() const { return p_outputSampleRate; }
	void ConsumeFrames(ma_uint64 frames) { p_globalFrameCount += frames; }


	void SetGlobalFrameCount(ma_uint64 frames) { p_globalFrameCount.store(frames); }
	void SetOutputSampleRate(ma_uint32 rate) { p_outputSampleRate = rate; }

	// 切换缓冲区并重置消耗帧数
	void SwitchBuffer();


private:
	void BufferFiller(ma_decoder* pDecoder);

	Buffer p_buffers[2];            // 双缓冲数组
	std::atomic<int> p_activeBuffer{0};  // 当前活动缓冲区索引
	std::atomic<ma_uint64> p_globalFrameCount{0}; // 全局已播放帧数
	ma_uint32 p_outputSampleRate = 0;    // 采样率（需初始化时获取）
	std::thread p_bufferFillerThread;    // 缓冲填充线程
	std::atomic<bool> p_keepFilling{true}; // 线程控制标志
};

#endif //BUFFERING_HPP