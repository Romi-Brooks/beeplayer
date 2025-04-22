/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Player.hpp
 *  Lib: Beeplayer Core engine Audio Player lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "../miniaudio/miniaudio.h"
#include <string>

// The Player class is a lightweight wrapper that encapsulates the public interfaces of the
// Decoder and Device classes for unified management
// and provides essential player functionalities.

class Player {
	private:
		float p_volume;
		std::string p_SongName;
	public:
		Player() : p_volume(1.0), p_SongName{} {}


	static void StaticCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
	void InstanceCallback(const ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);

	void Play(ma_device& Device, ma_decoder& Decode);

	std::string GetName() const { return p_SongName; }
	float GetVol() const { return p_volume; }
	void SetName(const std::string& name) { p_SongName = name; }
	void SetVol(float vol) { p_volume = vol; }
};



#endif //PLAYER_HPP
