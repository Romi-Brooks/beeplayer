/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: MusicPlayerState.cpp
 *  Lib: Beeplayer Player Outer Wrapper -> Audio Player Monitor and Controller
 *  Author: Romi Brooks
 *  Date: 2025-06-1
 *  Type: Wrapper, Core Engine
 */
#include "MusicPlayerState.hpp"

// Standard Lib
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <string>

// Basic Lib
#include "../Engine/DataCallback.hpp"
#include "../Log/LogSystem.hpp"

bool InitializeAudioPlayer(MusicPlayerState& state, const std::string& rootPath){
    std::lock_guard<std::mutex> lock(state.audioMutex);

    try {
    	// Creat Global Objects! [Important for this]
        state.Pather = new Path(rootPath);
        state.Decoder = new AudioDecoder();
        state.Device = &AudioDevice::GetDeviceInstance();
        state.Player = new AudioPlayer();

        // First: Init the Decoder
        state.Player->InitDecoder(*state.Pather, *state.Decoder);

        // Second: Init the Timer Counter and Double Buffering
        state.Timer = new Status(*state.Decoder);
        state.Buffer = new AudioBuffering(&state.Decoder->GetDecoder());

        // Third: Init the Player
        state.Player->InitDevice(*state.Decoder, *state.Device, data_callback, *state.Buffer);

        // Get the all media files in this Path
        auto files = state.Pather->GetFiles();
        for (const auto& file : files) {
            state.tracks.push_back(file);
        }

        if (state.tracks.empty()) {
            LOG_ERROR("Music Player State -> No Media File founded!");
            return false;
        }

        // This Thread is using for switch the song, when the file is play done.
        state.nextCheckThread = std::thread([&] {
            while (state.running) { // Check is this Status is Running?
                if (state.initialized && state.Player && state.Buffer && state.Timer &&
                    state.Pather && state.Decoder && state.Device) { // if running
                    state.Player->NextFileCheck(
                        *state.Buffer, *state.Timer, *state.Pather,
                        *state.Decoder, *state.Device, data_callback
                    ); // run this -> check the music play status, if play done, switch it.
                }
            }
        });

    	// This thread is using for player the music file (if it is init)
        state.playThread = std::thread([&] {
            if (state.initialized && state.Player && state.Device &&
                state.Decoder && state.Timer && state.Buffer) { // if inited the all things
                state.Player->Play(
                    state.Device->GetDevice(),
                    state.Decoder->GetDecoder(),
                    *state.Timer,
                    *state.Buffer
                ); // play it
            }
        });

        state.initialized = true;
        return true;
    } catch (const std::exception& e) {
    	std::stringstream ss;
        ss << "Music Player State -> Init the Audio Player Error: " << e.what() << std::endl;
    	LOG_ERROR(ss.str());
        return false;
    }
}

void CleanupAudioPlayer(MusicPlayerState& state) {
	state.running = false;

	if (state.nextCheckThread.joinable()) state.nextCheckThread.join();
	if (state.playThread.joinable()) state.playThread.join();

	std::lock_guard<std::mutex> lock(state.audioMutex);

	if (state.initialized) {
		if (state.Player && state.Device) {
			state.Player->Exit(*state.Device, *state.Decoder);
		}

		delete state.Pather;
		delete state.Decoder;
		delete state.Player;
		delete state.Timer;
		delete state.Buffer;

		state.initialized = false;
	}
}