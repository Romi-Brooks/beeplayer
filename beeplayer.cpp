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
#include "FileSystem/Path.hpp"



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
void ListenEvent(AudioPlayer& Player, AudioDevice& Device, AudioDecoder& Decoder) {
    std::cout << "Press e to Exit, p to Pause, n to next, u to Prev." << std::endl;
    while (true) {
        // 非阻塞检查用户输入
        if (IsInputAvailable()) {
            char key;
            std::cin >> key;
            switch (tolower(key)) {
                case 'e': {
                    Player.Exit(Device.GetDevice(),Decoder.GetDecoder());
                    exit(0);
                }
                case 'p': {
                    if (ma_device_is_started(&Device.GetDevice())) {
                        ma_device_stop(&Device.GetDevice());
                        std::cout << "Paused." << std::endl;
                    } else {
                        ma_device_start(&Device.GetDevice());
                        std::cout << "Resumed." << std::endl;
                    }
                    break;
                }
                case 'n': {
                    // SwitchMusic(Decoder.GetDecoder());
                    // InitEngineWithoutDevice(NextFile(), decoder, deviceConfig, device);
                    break;
                }
                case 'u': {
                    // SwitchMusic(decoder);
                    // InitEngineWithoutDevice(PreFile(), decoder, deviceConfig, device);
                    break;
                }
                default: {
                    std::cout << "Unknown Command." << std::endl;
                    break;
                }
            }
        }
    }
}

int main() {
	Path Pather("E:/Music");
	AudioDecoder Decoder;
	AudioDevice& Device = AudioDevice::GetDeviceInstance();
	AudioPlayer Player;

	// First: Init the Decoder From the file
	Decoder.InitDecoder(Pather.CurrentFilePath());
	Player.SetName(Path::GetFileName(Pather.CurrentFilePath()));

	// ma_uint64 TotalFrames = 0;
	// ma_data_source_get_length_in_pcm_frames(&Decoder.GetDecoder(), &TotalFrames);

	AudioBuffering DoubleBuffering(&Decoder.GetDecoder());	// 创建双缓冲实例并关联到设备

	// Second: Init the Device to make sure there is a device to play the audio
	Device.InitDeviceConfig(ma_standard_sample_rate_44100, ma_format_f32, data_callback, Decoder.GetDecoder(),&DoubleBuffering);
	Device.InitDevice(Decoder.GetDecoder());



	// Third: Play the audio
	LOG_INFO("Now Playing: " + Player.GetName());

	Player.Play(Device.GetDevice(), Decoder.GetDecoder());

	ListenEvent(Player, Device, Decoder);

	std::cin.get();
	return 0;
}

