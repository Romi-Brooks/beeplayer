/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Buffering.cpp
 *  Lib: Beeplayer Core engine Audio Buffer -> Double buffering lib
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: Core Engine
 */

#include <mutex>

#include "Buffering.hpp"


AudioBuffering::AudioBuffering(ma_decoder *decoder) {
	p_outputSampleRate = decoder->outputSampleRate;
	p_keepFilling = true;
	p_bufferFillerThread = std::thread(&AudioBuffering::BufferFiller, this, decoder);
}
AudioBuffering::~AudioBuffering() {
	p_keepFilling.store(false);
	if (p_bufferFillerThread.joinable()) {
		p_bufferFillerThread.join();
	}
}
void AudioBuffering::SwitchBuffer() { p_activeBuffer.store((p_activeBuffer.load() + 1) % 2); }

void AudioBuffering::BufferFiller(ma_decoder *pDecoder) {
	int nextBuffer = 0;
	while (p_keepFilling) {
		// 等待当前缓冲区消耗过半再填充
		if (p_buffers[nextBuffer].s_ready) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		// 计算需要读取的帧数（示例：500ms的缓冲）
		const ma_uint32 targetFrames = p_outputSampleRate * 0.5;
		p_buffers[nextBuffer].s_data.resize(targetFrames * ma_get_bytes_per_frame(pDecoder->outputFormat, pDecoder->outputChannels));

		// 读取音频数据
		ma_uint64 framesRead;
		ma_result result = ma_decoder_read_pcm_frames(
			pDecoder,
			p_buffers[nextBuffer].s_data.data(),
			targetFrames,
			&framesRead
		);

		if (result == MA_SUCCESS && framesRead > 0) {
			p_buffers[nextBuffer].s_startFrame = p_globalFrameCount;
			p_buffers[nextBuffer].s_totalFrames = framesRead;
			p_buffers[nextBuffer].s_ready = true;
			nextBuffer = (nextBuffer + 1) % 2;
		} else {
			// 处理文件结束或错误
			break;
		}
	}
}