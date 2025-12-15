/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Player.cpp
 *  Lib: Beeplayer Core engine Wrapper -> Abstract Low-Level controller
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Wrapper, Player, Core Engine
 */

#include "Player.hpp"

// Standard Lib
#include <thread>

// Basic Lib
#include "Buffering.hpp"
#include "../miniaudio/miniaudio.h"
#include "../Log/LogSystem.hpp"


void AudioPlayer::Play(AudioDevice &Device, AudioDecoder &Decoder, Status &Timer, AudioBuffering &Buffer) const {
	if (ma_device_start(&Device.GetDevice()) != MA_SUCCESS) {
		Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_PLAYER, "Error when play the file.");
             return;
	}

	Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_PLAYER, "Now Playing: ", this->GetName());
}

void AudioPlayer::Pause(AudioDevice &Device) {
	if (ma_device_is_started(&Device.GetDevice())) {
		ma_device_stop(&Device.GetDevice());
	} else {
		ma_device_start(&Device.GetDevice());
	}
}

void AudioPlayer::Seek(AudioDecoder &Decoder,ma_uint64 FrameIndex) {
	ma_decoder_seek_to_pcm_frame(&Decoder.GetDecoder(),FrameIndex);
}

void AudioPlayer::InitDecoder(const Path& Pather, AudioDecoder& Decoder) {
	Decoder.InitDecoder(Pather.CurrentFilePath());
	SetName(Path::GetFileName(Pather.CurrentFilePath()));
}

void AudioPlayer::InitDevice(AudioDecoder& Decoder, AudioDevice &Device, const ma_device_data_proc &Callback,
							 AudioBuffering &Buffer) {
	Device.InitDeviceConfig(Decoder.GetDecoder().outputSampleRate, Decoder.GetDecoder().outputFormat, Callback, Decoder.GetDecoder(), &Buffer);
	Device.InitDevice(Decoder.GetDecoder());
}

void AudioPlayer::Switch(Path &Pather, AudioDecoder &Decoder, AudioDevice &Device, const ma_device_data_proc &Callback,
						 Status &Timer, AudioBuffering &Buffer, SwitchAction SwitchCode) {
	switch (SwitchCode) {
		// Set To the next file.
		case SwitchAction::NEXT: {
			Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_PLAYER, "Switch Next!");
			Pather.NextFilePath();
			SetName(Path::GetFileName(Pather.CurrentFilePath()));
			break;
		}
		// Set To the previous file.
		case SwitchAction::PREV: {
			Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_PLAYER, "Switch Pre!");
			Pather.PrevFilePath();
			SetName(Path::GetFileName(Pather.CurrentFilePath()));
			break;
		}
		case SwitchAction::SPECIFIC:
			// using for Switch the specific index's song (aka: user's choice in ui)
			Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_PLAYER, "Jump To Selected!");
			SetName(Path::GetFileName(Pather.CurrentFilePath()));
			break;
		default: {
			break; // No matter what, code should be 0-1
		}
	}

	// ZERO: Make sure the data user all cleaned.
	Clean(Buffer, Timer, Decoder, Device);


	// First: Init the Decoder From the file
	Decoder.InitDecoder(Pather.CurrentFilePath());
	Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_PLAYER, "Reinit Decoder completed.");

	// Second: Init the Device to make sure there is a device to play the audio
	Device.InitDeviceConfig(Decoder.GetDecoder().outputSampleRate, Decoder.GetDecoder().outputFormat, Callback,
							Decoder.GetDecoder(), &Buffer);
	Device.InitDevice(Decoder.GetDecoder());
	Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_PLAYER, "Reinit the Device completed.");

	// Rerun the Time Counter and double buffering progress
	Timer.SetFileLength(Decoder); // reset the file length
	Buffer.GetBufferThread() = std::thread(&AudioBuffering::BufferFiller, &Buffer, &Decoder.GetDecoder());
	Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_PLAYER, "Rerun the double buffering progress.");

	// Third: Just Playing the file from decoder and device
	Play(Device, Decoder, Timer, Buffer);
	Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_PLAYER, "Start Playing.");
}

// This function write for switch actions,
void AudioPlayer::Clean(AudioBuffering &Buffer, Status& Timer , AudioDecoder &Decoder, AudioDevice &Device){
	ma_device_uninit(&Device.GetDevice());
	ma_decoder_uninit(&Decoder.GetDecoder());
	Timer.ResetStatus();
	Buffer.ResetBuffer();
}

// If the exec will be exited, try to this function call
void AudioPlayer::Exit(AudioDevice &Device, AudioDecoder &Decoder) {
	ma_device_stop(&Device.GetDevice());
	ma_device_uninit(&Device.GetDevice());
	ma_decoder_uninit(&Decoder.GetDecoder());
}
