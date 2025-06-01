/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Player.hpp
 *  Lib: Beeplayer Core engine Wrapper definitions -> Abstract Low-Level controller
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Wrapper, Player, Core Engine
 */

#ifndef PLAYER_HPP
#define PLAYER_HPP

// Basic Lib
#include "Status.hpp"
#include "Decoder.hpp"
#include "Device.hpp"

#include "../FileSystem/Path.hpp"

// The Player class is a lightweight wrapper that encapsulates the public interfaces of the
// Decoder and Device classes for unified management
// and provides essential player functionalities.

enum SwitchAction {
	NEXT, PREV, SPECIFIC
};

class AudioPlayer {
	public:
		AudioPlayer() : p_volume(1.0), p_SongName{}, p_audioBuffer(nullptr) {}
		~AudioPlayer() {
			delete p_audioBuffer;
		}

		// static void StaticCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
		// void InstanceCallback(const ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);

		void Play(ma_device &Device, ma_decoder &Decoder, Status& Timer, AudioBuffering& Buffer) const;

		std::string GetName() const { return p_SongName; }
		float GetVol() const { return p_volume; }
		void SetName(const std::string& name) { p_SongName = name; }
		void SetVol(float vol) { p_volume = vol; }

		void InitDecoder(const Path &Pather, AudioDecoder &Decoder);
		void InitDevice(AudioDecoder& Decoder, AudioDevice& Device, const ma_device_data_proc &Callback, AudioBuffering &Buffer);
		void Switch(Path& Pather, AudioDecoder& Decoder, AudioDevice& Device, const ma_device_data_proc &Callback, Status& Timer, AudioBuffering& Buffer, SwitchAction SwitchCode);
		// This Function is using for switch the song, when the file is play done.
		void NextFileCheck(AudioBuffering& Buffer, Status& Timer, Path& Pather, AudioDecoder& Decoder, AudioDevice& Device, const ma_device_data_proc &Callback);
		void Clean(AudioBuffering& Buffer, Status& Timer, AudioDecoder& Decoder, AudioDevice& Device);
		void Exit(ma_device& device, ma_decoder& decoder);

	private:
		float p_volume;
		std::string p_SongName;
		AudioBuffering* p_audioBuffer;  // Double Buffing Manager
		ma_uint64 p_consumedFrames = 0; // Played frames
};
#endif //PLAYER_HPP