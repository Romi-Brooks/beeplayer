/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Status.cpp
 *  Lib: Beeplayer Core engine Progress Status
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: Core Engine
 */

#include <iostream>
#include <chrono>
#include <thread>

#include "Status.hpp"

Status::Status(AudioDecoder &Decoder) {
	SetFileLength(Decoder);
}

void Status::SetFileLength(AudioDecoder &Decoder) {
	ma_uint64 temp_fileTotalFrames = 0;
	ma_data_source_get_length_in_pcm_frames(&Decoder.GetDecoder(), &temp_fileTotalFrames); // get the file's total frame
	p_fileTotalFrames.store(temp_fileTotalFrames); // Store the file's total frames
}

void Status::ProgressThread(AudioDecoder &Decoder, AudioBuffering &Buffer) {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		{
			const auto current = Buffer.GetGlobalFrameCount();
			const auto total = p_fileTotalFrames.load();

			const double currentTime = current / Decoder.GetDecoder().outputSampleRate;
			const double totalTime = total / Decoder.GetDecoder().outputSampleRate;

			std::cout << "\rNow Playing:" << std::setfill('0') << std::setw(2) << static_cast<int>(currentTime) / 60
					  << ":" << std::setfill('0') << std::setw(2) << static_cast<int>(currentTime) % 60 << "/"
					  << std::setfill('0') << std::setw(2) << static_cast<int>(totalTime) / 60 << ":"
					  << std::setfill('0') << std::setw(2) << static_cast<int>(totalTime) % 60 << "  \r"
					  << std::flush; // 仅刷新，不换行
		}
	}
}
void Status::ResetStatus(){
	this->p_fileTotalFrames.store(0);
}
