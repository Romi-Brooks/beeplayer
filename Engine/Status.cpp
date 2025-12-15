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

void Status::ResetStatus(){
	this->p_fileTotalFrames.store(0);
}