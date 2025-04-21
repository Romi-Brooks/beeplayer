#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

#pragma once
#include <string>
#include <functional>

#include "../miniaudio/miniaudio.h"

class AudioPlayer {
    public:
        // Class Default Constructor and Deconstructor
        AudioPlayer();
        ~AudioPlayer();

        // Audio Player Init and Uninit func
        bool AudioPlayerInit();
        void AudioPlayerUnInit();

        // Basic User Interface
        bool LoadFile(const std::string& FilePath);

        void Play();
        void Pause();
        void Stop();

        bool IsPlaying() const;
        bool IsPaused() const;

        void SetVolume(float I_Volume);

        float GetVolume() const;
        void SetName(const std::string& FilePath);
        std::string GetName() const;


        // Callback
        // CallbackFunction：一个 std::function<void()> 类型的参数，表示当音频播放结束时要调用的函数。这个函数没有参数，也没有返回值。
        void SetOnPlaybackFinishedCallback(const std::function<void()> &CallbackFunction);

    private:
        ma_engine PlayerEngine;
        ma_sound PlayerSound;

        std::function<void()> OnPlaybackFinishedCallback;

        bool Playing = false;
        bool Paused = false;
        float Volume = 1.0f;
        std::string SongName;

        // pSound：指向当前播放的 ma_sound 对象的指针。这个对象包含了音频播放的相关信息。
        // pUserData：用户数据指针，用于传递自定义数据。它被用来传递 AudioPlayer 对象的指针，以便在回调中访问 AudioPlayer 的成员函数和数据。
        static void PlaybackFinishedCallback(ma_sound* PlayerSound, void* PlayerUserData);
};

#endif //AUDIOPLAYER_HPP
