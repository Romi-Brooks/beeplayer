/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Player.cpp
 *  Lib: Beeplayer Core engine Audio Player lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#include "Player.hpp"
#include <iostream>
#include <cstring>

// // Prototype Callback:
// void* Player::Callback(const ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
// 	auto *pDecoder = static_cast<ma_decoder *>(pDevice->pUserData);
// 	if (pDecoder == nullptr) {
// 		std::cout << "Error when load the callback";
// 	}
// 	ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, nullptr);
// 	(void) pInput;
// }

void AudioPlayer::StaticCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	// 从用户数据获取Player实例
	auto* player = static_cast<AudioPlayer*>(pDevice->pUserData);
	player->InstanceCallback(pDevice, pOutput, pInput, frameCount);
}

// void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
//     static ma_uint64 consumedFrames = 0; // 当前缓冲区已消耗帧数
//     const int currentBufIdx = activeBuffer.load();
//     AudioBuffer& currentBuf = buffers[currentBufIdx];
//
//     if (!currentBuf.ready) {
//         memset(pOutput, 0, frameCount * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels));
//         return;
//     }
//
//     // 计算可复制帧数
//     const ma_uint64 availableFrames = currentBuf.totalFrames - consumedFrames;
//     const ma_uint64 framesToCopy = std::min(static_cast<ma_uint64>(frameCount), availableFrames);
//
//     // 复制数据到输出缓冲区
//     const size_t bytesToCopy = framesToCopy * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
//     memcpy(
//         pOutput,
//         currentBuf.data.data() + consumedFrames * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels),
//         bytesToCopy
//     );
//
//     // 更新计数器
//     consumedFrames += framesToCopy;
//     globalFrameCount += framesToCopy;
//
//     // 处理缓冲区切换
//     if (consumedFrames >= currentBuf.totalFrames) {
//         currentBuf.ready = false;
//         activeBuffer.store((currentBufIdx + 1) % 2);
//         consumedFrames = 0;
//     }
//
//     // 填充剩余空间（如果有）
//     if (framesToCopy < frameCount) {
//         const size_t remainingBytes = (frameCount - framesToCopy) * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
//         memset(static_cast<char *>(pOutput) + bytesToCopy, 0, remainingBytes);
//     }
// }
//
void AudioPlayer::InstanceCallback(const ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
	// auto *pDecoder = static_cast<ma_decoder *>(pDevice->pUserData);
	// if (pDecoder == nullptr) {
	// 	std::cout << "Error when load the callback";
	// }
	// ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, nullptr);
	// (void) pInput;
	static ma_uint64 p_consumedFrames = 0;
	auto& currentBuf = p_audioBuffer->GetActiveBuffer();
	const ma_uint32 bytesPerFrame = ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);

	// 缓冲区未就绪时填充静音
	if (!currentBuf.ready) {
		memset(pOutput, 0, frameCount * bytesPerFrame);
		return;
	}

	// 计算实际可复制帧数
	const ma_uint64 availableFrames = currentBuf.s_totalFrames - p_consumedFrames;
	const ma_uint64 framesToCopy = std::min(static_cast<ma_uint64>(frameCount), availableFrames);
	const size_t bytesToCopy = framesToCopy * bytesPerFrame;

	// 复制音频数据
	memcpy(pOutput, currentBuf.s_data.data() + p_consumedFrames * bytesPerFrame, bytesToCopy);

	// 更新计数状态
	p_consumedFrames += framesToCopy;
	p_audioBuffer->AddGlobalFrames(framesToCopy);


	// 处理缓冲区耗尽
	if (p_consumedFrames >= currentBuf.s_totalFrames) {
		currentBuf.ready = false;
		p_audioBuffer->SwitchBuffer();

		p_consumedFrames = 0; // 重置为下一个缓冲区准备
	}

	// 填充剩余空间（静音）
	if (framesToCopy < frameCount) {
		const size_t remainingBytes = (frameCount - framesToCopy) * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
		memset(static_cast<char*>(pOutput) + bytesToCopy, 0, remainingBytes);
	}
}

void AudioPlayer::Play(ma_device &Device, ma_decoder &Decoder)  {
	if (ma_device_start(&Device) != MA_SUCCESS) {
		std::cout << "Error when play the file.";
		ma_device_uninit(&Device);
		ma_decoder_uninit(&Decoder);
	}
	std::cout << "Playing...";
}
