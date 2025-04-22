/*
 *  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *
 *  Lib: Beeplayer Core engine Audio Device lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Audio Core Engine
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
            ma_device_uninit(&p_device);
        }

    public:
    // Singleton Instance
    static AudioDevice& GetDeviceInstance() {
        static AudioDevice device;
        return device;
    }

    ma_device& GetDevice() {
        return this->p_device;
    }

    void InitDeviceConfig(const ma_uint32& SampleRate, const ma_format& Format,const ma_device_data_proc& Callback, ma_decoder& Decoder) {
        p_deviceConfig = ma_device_config_init(ma_device_type_playback);
        p_deviceConfig.playback.format   = Format;
        // Device channels equal 2, indicating stereo.
        // Actually, No one can use this pieces of shit exec to listen 7.1 or something right..?
        p_deviceConfig.playback.channels = 2;
        // Under normal conditions, the sample rate is 44.1 kHz or 48.0 kHz.
        // But when tracks are switched, the sample rate may change.
        // This can cause problems. To solve this, pass the sample rate explicitly.
        p_deviceConfig.sampleRate        = SampleRate;
        p_deviceConfig.dataCallback      = Callback;   // CallBack Function
        p_deviceConfig.pUserData         = &Decoder;   // Can be accessed from the device object (device.pUserData).
    }

    void InitDevice(ma_decoder& Decoder) {
        if (ma_device_init(nullptr, &this->p_deviceConfig, &p_device) != MA_SUCCESS) {
            std::cout << "Error to init device." << std::endl;
            ma_decoder_uninit(&Decoder); // For Safety
        }
    }

    // Makesure the class is a singleton
    AudioDevice(const AudioDevice&) = delete;
    void operator=(const AudioDevice&) = delete;
};

#endif //AUDIODEVICE_HPP
