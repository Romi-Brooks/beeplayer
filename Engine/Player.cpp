/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Player.cpp
 *  Lib: Beeplayer Core engine Audio Player lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#include <iostream>

#include "Player.hpp"

#include "../Log/LogSystem.hpp"

// void AudioPlayer::StaticCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
// 	// 从用户数据获取Player实例
// 	auto* player = static_cast<AudioPlayer*>(pDevice->pUserData);
// 	player->InstanceCallback(pDevice, pOutput, pInput, frameCount);
// }

// Another Callback func
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
// void AudioPlayer::InstanceCallback(const ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
// 	// auto *pDecoder = static_cast<ma_decoder *>(pDevice->pUserData);
// 	// if (pDecoder == nullptr) {
// 	// 	std::cout << "Error when load the callback";
// 	// }
// 	// ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, nullptr);
// 	// (void) pInput;
//     const ma_uint32 bytesPerFrame =ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels); /* 计算字节数 */;
//     auto& currentBuf = p_audioBuffer->GetActiveBuffer();
//
//     // 带重试的缓冲区检查
//     if (!currentBuf.CheckReady()) {
//         constexpr int maxRetry = 5;
//         for (int i = 0; i < maxRetry; ++i) {
//             std::this_thread::yield();
//             if (currentBuf.CheckReady()) break;
//         }
//         if (!currentBuf.CheckReady()) {
//             memset(pOutput, 0, frameCount * bytesPerFrame);
//             return;
//         }
//     }
//
//     // 安全数据拷贝
//     const ma_uint64 safeFrames = std::min(
//         static_cast<ma_uint64>(frameCount),
//         currentBuf.s_totalFrames - p_consumedFrames
//     );
//
//     if (safeFrames > 0) {
//         const size_t copyBytes = safeFrames * bytesPerFrame;
//         const size_t dataOffset = p_consumedFrames * bytesPerFrame;
//
//         // 增加边界检查
//         if (dataOffset + copyBytes <= currentBuf.s_data.size()) {
//             memcpy(pOutput, currentBuf.s_data.data() + dataOffset, copyBytes);
//         } else {
//             memset(pOutput, 0, copyBytes);
//         }
//
//         p_consumedFrames += safeFrames;
//         p_audioBuffer->AddGlobalFrames(safeFrames);
//     }
//
//     // 处理缓冲区切换
//     if (p_consumedFrames >= currentBuf.s_totalFrames) {
//         const ma_uint64 overflow = p_consumedFrames - currentBuf.s_totalFrames;
//         currentBuf.ready.store(false, std::memory_order_release);
//         p_audioBuffer->SwitchBuffer();
//         p_consumedFrames = overflow;  // 保留溢出帧
//
//         // 处理连续溢出情况
//         if (overflow > 0 && p_audioBuffer->HasValidData()) {
//             auto& newBuf = p_audioBuffer->GetActiveBuffer();
//             const ma_uint64 validFrames = std::min(overflow, newBuf.s_totalFrames);
//             if (validFrames > 0) {
//                 memcpy(pOutput, newBuf.s_data.data(), validFrames * bytesPerFrame);
//                 p_consumedFrames = validFrames;
//             }
//         }
//     }
//
//     // 填充静音
//     const size_t filled = safeFrames * bytesPerFrame;
//     if (filled < frameCount * bytesPerFrame) {
//         memset(static_cast<uint8_t*>(pOutput) + filled, 0,
//               (frameCount * bytesPerFrame) - filled);
//     }
// }

void AudioPlayer::Play(ma_device &Device, ma_decoder &Decoder)  {
	if (ma_device_start(&Device) != MA_SUCCESS) {
		LOG_ERROR("Error when play the file.");
		ma_device_uninit(&Device);
		ma_decoder_uninit(&Decoder);
	}
}
