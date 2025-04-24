/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Decoder.cpp
 *  Lib: Beeplayer Core engine Audio Decoder lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#include <iostream>

#include "Decoder.hpp"
#include "../Log/LogSystem.hpp"

ma_decoder & AudioDecoder::GetDecoder() {
    return this->p_decoder;
}

void AudioDecoder::InitDecoder(const std::string &FilePath) {
	ma_result result = ma_decoder_init_file(FilePath.c_str(), nullptr, &this->p_decoder);
	if (result != MA_SUCCESS) {
		LOG_ERROR("Error to loading the file:"  + FilePath);
	}
	LOG_INFO("Init Decoder Completed.");
}