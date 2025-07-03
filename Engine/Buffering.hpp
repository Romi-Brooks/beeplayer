/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Buffering.hpp
 *  Lib: Beeplayer Core engine Audio Buffer definitions -> Double buffering lib
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: Buffers, Core Engine
 */

#ifndef BUFFERING_HPP
#define BUFFERING_HPP

// Standard Lib
#include <atomic>
#include <thread>
#include <vector>

// Basic Lib
#include "../miniaudio/miniaudio.h"

class AudioBuffering {
	public:
		struct Buffer {
			std::vector<ma_uint8> s_data; // 原始音频数据
			ma_uint64 s_startFrame = 0; // 缓冲区起始帧位置
			ma_uint64 s_totalFrames = 0; // 缓冲区总帧数
			std::atomic<bool> s_ready{false}; // 缓冲区就绪状态
		};

	    explicit AudioBuffering(ma_decoder *decoder);

	    ~AudioBuffering();

		void BufferFiller(ma_decoder* pDecoder);

		// Getter
		Buffer* GetBuffers() { return p_buffers; }
		std::thread& GetBufferThread() { return p_bufferFillerThread; }
		int GetActiveBuffer() const { return p_activeBuffer.load(); }
		ma_uint64 GetGlobalFrameCount() const { return p_globalFrameCount.load(); }
		ma_uint32 GetOutputSampleRate() const { return p_outputSampleRate; }

		void SwitchBuffer();
		void ConsumeFrames(ma_uint64 frames) { p_globalFrameCount += frames; }

		void SetGlobalFrameCount(ma_uint64 frames) { p_globalFrameCount.store(frames); }
		void SetOutputSampleRate(ma_uint32 rate) { p_outputSampleRate = rate; }

		void ResetBuffer();

	private:
		Buffer p_buffers[2];            // 双缓冲数组
		std::atomic<int> p_activeBuffer{0};  // 当前活动缓冲区索引
		std::atomic<ma_uint64> p_globalFrameCount{0}; // 全局已播放帧数
		ma_uint32 p_outputSampleRate = 0;    // 采样率（需初始化时获取）
		std::thread p_bufferFillerThread;    // 缓冲填充线程
		std::atomic<bool> p_keepFilling{true}; // 线程控制标志
};

#endif //BUFFERING_HPP