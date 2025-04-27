/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Player.cpp
 *  Lib: Beeplayer Core engine Audio Player lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#include <iostream>
#include <thread>

#include "../miniaudio/miniaudio.h"
#include "../Log/LogSystem.hpp"
#include "../FileSystem/Path.hpp"

#include "Player.hpp"
#include "Buffering.hpp"

void AudioPlayer::Play(ma_device &Device, ma_decoder &Decoder, Status& Timer, AudioBuffering& Buffer) const {
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

void AudioPlayer::InitDevice(AudioDecoder& Decoder, AudioDevice &Device, const ma_device_data_proc &Callback,
							 AudioBuffering &Buffer) {
	Device.InitDeviceConfig(ma_standard_sample_rate_44100, ma_format_f32, Callback, Decoder.GetDecoder(), &Buffer);
	Device.InitDevice(Decoder.GetDecoder());
}

void AudioPlayer::Switch(Path &Pather, AudioDecoder &Decoder, AudioDevice &Device, const ma_device_data_proc &Callback,
						 Status &Timer, AudioBuffering &Buffer, SwitchAction SwitchCode) {
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
		default: {
			break; // No matter what, code should be 0-1
		}
	}

	// ZERO: Make sure the data user all cleaned.
	Clean(Buffer, Timer, Decoder, Device);

	// First: Init the Decoder From the file
	Decoder.InitDecoder(Pather.CurrentFilePath());
	LOG_INFO("Audio Player-> Device init completed.");

	// Second: Init the Device to make sure there is a device to play the audio
	Device.InitDeviceConfig(Decoder.GetDecoder().outputSampleRate, Decoder.GetDecoder().outputFormat, Callback,
							Decoder.GetDecoder(), &Buffer);
	Device.InitDevice(Decoder.GetDecoder());
	Timer.SetFileLength(Decoder); // reset the file length
	LOG_INFO("Audio Player-> Device init completed.");

	// Rerun the double buffering progress
	Buffer.GetBufferThread() = std::thread(&AudioBuffering::BufferFiller, &Buffer, &Decoder.GetDecoder());
	LOG_INFO("Buffer Thread -> Rerun the double buffering progress.");

	// Third: Just Playing the file from decoder and device
	Play(Device.GetDevice(), Decoder.GetDecoder(), Timer, Buffer);
	LOG_INFO("Audio Player -> Start Playing.");




	// Rerun the Time Counter progress
	// std::thread(&Status::ProgressThread, this, std::ref(Decoder), std::ref(Buffer)).detach();
	// LOG_INFO("Status Thread -> Rerun the double buffering progress.");
	// ListenEvent(decoder, deviceConfig, device); // No hpp file include
}
void AudioPlayer::NextFileCheck(AudioBuffering &Buffer, Status &Timer, Path &Pather, AudioDecoder &Decoder, AudioDevice &Device, const ma_device_data_proc &Callback) {
	while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
	{
			double currentTime = Buffer.GetGlobalFrameCount() / Decoder.GetDecoder().outputSampleRate;
			double totalTime = Timer.GetTotalFrames() / Decoder.GetDecoder().outputSampleRate;
			// std::cout << "Timer Checker -> Current Time/Total Time:" << currentTime << "/" << totalTime << std::endl; // Debugger
			if (totalTime > 0 && currentTime >= totalTime) {
				LOG_INFO("Player -> End of track, switching to next...");
				Switch(Pather, Decoder, Device, Callback, Timer, Buffer, SwitchAction::NEXT);
				// 重置计数器避免重复触发
				currentTime = 0;
				totalTime = 0;
			}
		}
	}
}

void AudioPlayer::Clean(AudioBuffering &Buffer, Status& Timer , AudioDecoder &Decoder, AudioDevice &Device){
	ma_device_uninit(&Device.GetDevice());
	ma_decoder_uninit(&Decoder.GetDecoder());
	Timer.ResetStatus();
	Buffer.ResetBuffer();
}

void AudioPlayer::Exit(ma_device &device, ma_decoder &decoder) {
	ma_device_uninit(&device);
	ma_decoder_uninit(&decoder);
}