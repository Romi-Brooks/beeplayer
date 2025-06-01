/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Device.cpp
 *  Lib: Beeplayer Core engine Audio Device lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Device, Core Engine
 */

#include "Device.hpp"

// Standard Lib
#include <iostream>
#include <sstream>

// Basic Lib
#include "../Log/LogSystem.hpp"

ma_device & AudioDevice::GetDevice() {
	return this->p_device;
}

void AudioDevice::InitDeviceConfig(const ma_uint32 &SampleRate, const ma_format &Format, const ma_device_data_proc &Callback, ma_decoder &Decoder, void* DoubleBuffering) {
	p_deviceConfig = ma_device_config_init(ma_device_type_playback);
    p_deviceConfig.playback.format   = Format;
    // Device channels equal 2, indicating stereo.
    // Actually, No one can use this pieces of shit exec to listen 7.1 or something right?
    p_deviceConfig.playback.channels = 2;
    // Under normal conditions, the sample rate is 44.1 kHz or 48.0 kHz.
    // When tracks are switched, the sample rate may change.
    // This can cause problems. To solve this, pass the sample rate explicitly.
    p_deviceConfig.sampleRate        = SampleRate;
    p_deviceConfig.dataCallback      = Callback;   // CallBack Function
    p_deviceConfig.pUserData         = DoubleBuffering;   // Can be accessed from the device object (device.pUserData).

	// LOG_INFO("Audio Device -> Device Config Initialized.");
	std::stringstream ss;
	ss << "Audio Device -> Init Device Completed with device's sample rate: " << p_deviceConfig.sampleRate << "Hz"
	   << " and device format: " << p_deviceConfig.playback.format;
	LOG_INFO(ss.str());
}

void AudioDevice::InitDevice(ma_decoder &Decoder) {
    if (ma_device_init(nullptr, &this->p_deviceConfig, &p_device) != MA_SUCCESS) {
    	LOG_ERROR("miniaudio -> Error to init the Device.");
        ma_decoder_uninit(&Decoder); // For Safety
    }
	LOG_INFO("Audio Device -> Device Initialized.");
}
