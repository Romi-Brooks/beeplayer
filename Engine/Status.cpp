/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Status.cpp
 *  Lib: Beeplayer Core engine Progress Status
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: Status, Core Engine
 */

#include "Status.hpp"

// Standard Lib
#include <thread>

Status::Status(AudioDecoder &Decoder) {
	SetFileLength(Decoder);
}

void Status::SetFileLength(AudioDecoder &Decoder) {
	ma_uint64 temp_fileTotalFrames = 0;
	ma_data_source_get_length_in_pcm_frames(&Decoder.GetDecoder(), &temp_fileTotalFrames); // get the file's total frame
	p_fileTotalFrames.store(temp_fileTotalFrames); // Store the file's total frames
}

// void Status::ProgressThread(AudioDecoder &Decoder, AudioBuffering &Buffer) {
// 	while (true) {
// 		std::this_thread::sleep_for(std::chrono::seconds(1));
// 		{
// 			const double currentTime = Buffer.GetGlobalFrameCount() / Decoder.GetDecoder().outputSampleRate;
// 			const double totalTime = p_fileTotalFrames.load() / Decoder.GetDecoder().outputSampleRate;
// 			std::cout << "\rNow Playing:" << std::setfill('0') << std::setw(2) << static_cast<int>(currentTime) / 60
// 					  << ":" << std::setfill('0') << std::setw(2) << static_cast<int>(currentTime) % 60 << "/"
// 					  << std::setfill('0') << std::setw(2) << static_cast<int>(totalTime) / 60 << ":"
// 					  << std::setfill('0') << std::setw(2) << static_cast<int>(totalTime) % 60 << "  \r"
// 					  << std::flush; // 仅刷新，不换行
// 		}
// 	}
// }

void Status::ResetStatus(){
	this->p_fileTotalFrames.store(0);
}