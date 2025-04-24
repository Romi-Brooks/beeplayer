/*
 *  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
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

#include <iostream>
#include <cstring>

#include "miniaudio/miniaudio.h"
#include "Engine/Buffering.hpp"
#include "Engine/Decoder.hpp"
#include "Engine/Device.hpp"
#include "Engine/Player.hpp"
#include "Log/LogSystem.hpp"




// 修改后的回调函数
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	auto* buffering = static_cast<AudioBuffering*>(pDevice->pUserData);
	static ma_uint64 consumedFrames = 0;

	const int currentBufIdx = buffering->GetActiveBuffer();
	AudioBuffering::Buffer& currentBuf = buffering->GetBuffers()[currentBufIdx];

	if (!currentBuf.s_ready) {
		memset(pOutput, 0, frameCount * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels));
		return;
	}

	const ma_uint64 availableFrames = currentBuf.s_totalFrames - consumedFrames;
	const ma_uint64 framesToCopy = std::min(static_cast<ma_uint64>(frameCount), availableFrames);

	const size_t bytesToCopy = framesToCopy * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
	memcpy(
		pOutput,
		currentBuf.s_data.data() + consumedFrames * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels),
		bytesToCopy
	);

	consumedFrames += framesToCopy;
	buffering->ConsumeFrames(framesToCopy);

	if (consumedFrames >= currentBuf.s_totalFrames) {
		currentBuf.s_ready = false;
		buffering->SwitchBuffer();
		consumedFrames = 0;
	}

	if (framesToCopy < frameCount) {
		const size_t remainingBytes = (frameCount - framesToCopy) * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
		memset(static_cast<char*>(pOutput) + bytesToCopy, 0, remainingBytes);
	}
}

int main() {

	AudioDecoder Decoder;
	AudioDevice& Device = AudioDevice::GetDeviceInstance();
	AudioPlayer Player;

	// First: Init the Decoder From the file
	Decoder.InitDecoder("media/蓝色的海-本兮.mp3");
	Player.SetName("蓝色的海 - 本兮");

	// ma_uint64 TotalFrames = 0;
	// ma_data_source_get_length_in_pcm_frames(&Decoder.GetDecoder(), &TotalFrames);

	AudioBuffering DoubleBuffering(&Decoder.GetDecoder());	// 创建双缓冲实例并关联到设备


	// Second: Init the Device to make sure there is a device to play the audio
	Device.InitDeviceConfig(ma_standard_sample_rate_44100, ma_format_f32, data_callback, Decoder.GetDecoder(),&DoubleBuffering);
	Device.InitDevice(Decoder.GetDecoder());



	// Third: Play the audio
	LOG_INFO("Now Playing: " + Player.GetName());

	Player.Play(Device.GetDevice(), Decoder.GetDecoder());

	std::cin.get();
	return 0;
}

