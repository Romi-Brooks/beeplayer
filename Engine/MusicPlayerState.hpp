/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: MusicPlayerState.hpp
 *  Lib: Beeplayer Player Outer Wrapper definitions -> Audio Player Monitor and Controller
 *  Author: Romi Brooks
 *  Date: 2025-06-1
 *  Type: Wrapper, Core Engine
 */

#ifndef MUSICPLAYERSTATE_HPP
#define MUSICPLAYERSTATE_HPP

// Basic Lib
#include "../Engine/Buffering.hpp"
#include "../Engine/Decoder.hpp"
#include "../Engine/Device.hpp"
#include "../Engine/Player.hpp"
#include "../Engine/Status.hpp"
#include "../FileSystem/Path.hpp"

struct MusicPlayerState {
	std::vector<std::string> tracks;
	int currentTrack = 0;
	std::atomic<bool> isPlaying{false};
	std::atomic<float> progress{0.0f};
	float volume = 0.8f;

	// Global Objects
	Path* Pather = nullptr;
	AudioDecoder* Decoder = nullptr;
	AudioDevice* Device = nullptr;
	AudioPlayer* Player = nullptr;
	Status* Timer = nullptr;
	AudioBuffering* Buffer = nullptr;

	// Thread Controller
	std::atomic<bool> running{true};
	std::thread nextCheckThread;
	std::thread playThread;

	// Mutex
	std::mutex audioMutex;

	// Status Flags
	bool initialized = false;
};

// Init Audio Player
bool InitializeAudioPlayer(MusicPlayerState& state, const std::string& rootPath);
// Clean Res
void CleanupAudioPlayer(MusicPlayerState& state);
#endif //MUSICPLAYERSTATE_HPP
