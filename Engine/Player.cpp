/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Player.cpp
 *  Lib: Beeplayer Core engine Audio Player lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#include <iostream>

#include "../miniaudio/miniaudio.h"

#include "Buffering.hpp"
#include "Player.hpp"

#include "../Log/LogSystem.hpp"
#include "../FileSystem/Path.hpp"

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

void AudioPlayer::Play(ma_device &Device, ma_decoder &Decoder) {
	if (ma_device_start(&Device) != MA_SUCCESS) {
		LOG_ERROR("miniaudio -> Error when play the file.");
		ma_device_uninit(&Device);
		ma_decoder_uninit(&Decoder);
	}
	LOG_INFO("Audio Player-> Now Playing: " + this->GetName());
}

void AudioPlayer::InitDecoder(Path& Pather, AudioDecoder& Decoder) {
	// 总是在主函数中首先调用
	Decoder.InitDecoder(Pather.CurrentFilePath());
	SetName(Path::GetFileName(Pather.CurrentFilePath()));
}

void AudioPlayer::InitDevice(AudioDecoder& Decoder, AudioDevice& Device, const ma_device_data_proc &Callback, AudioBuffering &Buffer) {
	Device.InitDeviceConfig(ma_standard_sample_rate_44100, ma_format_f32, Callback, Decoder.GetDecoder(), &Buffer);
	Device.InitDevice(Decoder.GetDecoder());
}

void AudioPlayer::Switch(Path &Pather, AudioDecoder &Decoder, AudioDevice &Device, const ma_device_data_proc &Callback, AudioBuffering &Buffer, SwitchAction SwitchCode) {
	switch (SwitchCode) {
		// Set To the next file.
		case SwitchAction::NEXT: {
			LOG_INFO("Audio Player-> Switch Next!");
			Pather.NextFilePath();
			SetName(Pather.GetFileName(Pather.CurrentFilePath()));
			break;
		}
		// Set To the previous file.
		case SwitchAction::PREV: {
			LOG_INFO("Audio Player-> Switch Pre!");
			Pather.PrevFilePath();
			SetName(Pather.GetFileName(Pather.CurrentFilePath()));
			break;
		}
		default: { break; } // No matter what, code should be 0-1
	}

	ma_device_uninit(&Device.GetDevice());
	ma_decoder_uninit(&Decoder.GetDecoder());
	// ZERO: Make sure the buffer thread is stopped.
	Buffer.ResetBuffer();

	// First: Init the Decoder From the file
	Decoder.InitDecoder(Pather.CurrentFilePath());
	LOG_INFO("Audio Player-> Device init completed.");

	// Second: Init the Device to make sure there is a device to play the audio
	Device.InitDeviceConfig(Decoder.GetDecoder().outputSampleRate, Decoder.GetDecoder().outputFormat, Callback, Decoder.GetDecoder(), &Buffer);
	Device.InitDevice(Decoder.GetDecoder());
	LOG_INFO("Audio Player-> Device init completed.");

	// Third: Just Playing the file from decoder and device
	Play(Device.GetDevice(), Decoder.GetDecoder());
	LOG_INFO("Audio Player -> Start Playing.");

	// Rerun the double buffering progress
	Buffer.GetBufferThread() = std::thread(&AudioBuffering::BufferFiller, &Buffer, &Decoder.GetDecoder());
	LOG_INFO("Audio Player -> Rerun the double buffering progress.");
	// ListenEvent(decoder, deviceConfig, device); // No hpp file include
}

void AudioPlayer::Exit(ma_device &device, ma_decoder &decoder) {
	ma_device_uninit(&device);
	ma_decoder_uninit(&decoder);
}
