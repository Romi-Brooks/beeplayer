/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
*  File Name: Device.hpp
 *  Lib: Beeplayer Core engine Audio Device lib definitions
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Device, Core Engine
 */

#ifndef AUDIODEVICE_HPP
#define AUDIODEVICE_HPP

// Basic Lib
#include "../miniaudio/miniaudio.h"

class AudioDevice {
public:
	// Singleton Instance
	static AudioDevice& GetDeviceInstance() {
		static AudioDevice device;
		return device;
	}

	// Makesure the class is a singleton
	AudioDevice(const AudioDevice&) = delete;
	void operator=(const AudioDevice&) = delete;

	ma_device& GetDevice();

	void InitDeviceConfig(const ma_uint32& SampleRate, const ma_format& Format,const ma_device_data_proc& Callback, ma_decoder& Decoder, void* DoubleBuffering);
	void InitDevice(ma_decoder& Decoder);

private:
	ma_device p_device;
	ma_device_config p_deviceConfig;

	// Default constructor
	AudioDevice() : p_device{}, p_deviceConfig{} {}

	// Default Destructor
	~AudioDevice() {
		ma_device_uninit(&p_device); // Free Device
	}
};

#endif //AUDIODEVICE_HPP