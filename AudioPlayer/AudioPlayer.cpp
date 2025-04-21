
#include "AudioPlayer.hpp"

#include <iostream>
// Class Default Constructor and Deconstructor
AudioPlayer::AudioPlayer() : PlayerEngine{}, PlayerSound{}, SongName{} {
    if (ma_engine_init(nullptr, &PlayerEngine) != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
    }
}

AudioPlayer::~AudioPlayer() {
    AudioPlayerUnInit();
}

// Audio Player Init and Uninit func
bool AudioPlayer::AudioPlayerInit() {
    if (ma_engine_init(nullptr, &PlayerEngine) != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
        return false;
    }
    return true;
}

void AudioPlayer::AudioPlayerUnInit() {
    ma_engine_uninit(&PlayerEngine);
}

// Basic User Interface
bool AudioPlayer::LoadFile(const std::string& FilePath) {
    this->SetName(FilePath);
    if (ma_sound_init_from_file(&PlayerEngine, FilePath.c_str(), 0, nullptr, nullptr, &PlayerSound) != MA_SUCCESS) {
        std::cerr << "Failed to load sound file: " << FilePath << std::endl;
        return false;
    }
    return true;
}

void AudioPlayer::Play() {
    if (ma_sound_start(&PlayerSound) != MA_SUCCESS) {
        std::cerr << "Failed to start sound." << std::endl;
        return;
    }
    Playing = true;
    Paused = false;
}

void AudioPlayer::Pause() {
    if (ma_sound_stop(&PlayerSound) != MA_SUCCESS) {
        std::cerr << "Failed to pause sound." << std::endl;
        return;
    }
    Playing = false;
    Paused = true;
}

void AudioPlayer::Stop() {
    if (ma_sound_stop(&PlayerSound) != MA_SUCCESS) {
        std::cerr << "Failed to stop sound." << std::endl;
        return;
    }
    ma_sound_seek_to_pcm_frame(&PlayerSound, 0);
    Playing = false;
    Paused = false;
}

bool AudioPlayer::IsPlaying() const {
    return Playing;
}

bool AudioPlayer::IsPaused() const {
    return Paused;
}

void AudioPlayer::SetVolume(float I_Volume) {
    this->Volume = I_Volume;
    ma_sound_set_volume(&PlayerSound, I_Volume);
}

float AudioPlayer::GetVolume() const {
    return this->Volume;
}

void AudioPlayer::SetName(const std::string& FilePath) {
    // 提取文件名
    size_t lastSlashPos = FilePath.find_last_of("/\\");
    std::string FileName;
    if (lastSlashPos != std::string::npos) {
        FileName = FilePath.substr(lastSlashPos + 1);
    } else {
        FileName = FilePath;
    }

    // 去掉文件后缀
    size_t lastDotPos = FileName.find_last_of('.');
    if (lastDotPos != std::string::npos && lastDotPos > 0) {
        this->SongName = FileName.substr(0, lastDotPos);
    } else {
        this->SongName = FileName;
    }
}

std::string AudioPlayer::GetName() const {
    return this->SongName;
}

// Callback Functions
void AudioPlayer::SetOnPlaybackFinishedCallback(const std::function<void()> &CallbackFunction) {
    OnPlaybackFinishedCallback = CallbackFunction;
}

void AudioPlayer::PlaybackFinishedCallback(ma_sound* PlayerSound, void* PlayerUserData) {
    auto* Player = static_cast<AudioPlayer*>(PlayerUserData);
    if (Player->OnPlaybackFinishedCallback) {
        Player->OnPlaybackFinishedCallback();
    }
}
