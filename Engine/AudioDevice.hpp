/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: AudioDeice.hpp
 *  Lib: Beeplayer Core engine Audio Device lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#ifndef AUDIODEVICE_HPP
#define AUDIODEVICE_HPP
#include "../miniaudio/miniaudio.h"
#include <iostream>

class AudioDevice {
    private:
        ma_device p_device;
        ma_device_config p_deviceConfig;

        // Default constructor
        AudioDevice() : p_device{}, p_deviceConfig{} {}

        // Default Destructor
        ~AudioDevice() {
            ma_device_uninit(&p_device); // Free Device
        }

    public:
    // Singleton Instance
    static AudioDevice& GetDeviceInstance() {
        static AudioDevice device;
        return device;
    }

    ma_device& GetDevice();

    void InitDeviceConfig(const ma_uint32& SampleRate, const ma_format& Format,const ma_device_data_proc& Callback, ma_decoder& Decoder);

    void InitDevice(ma_decoder& Decoder);

    // Makesure the class is a singleton
    AudioDevice(const AudioDevice&) = delete;
    void operator=(const AudioDevice&) = delete;
};

#endif //AUDIODEVICE_HPP
