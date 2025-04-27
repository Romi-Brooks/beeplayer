/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Decoder.cpp
 *  Lib: Beeplayer Core engine Audio Decoder lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

// #include <iostream>

#include "Decoder.hpp"

#include <iostream>

#include "../Log/LogSystem.hpp"

ma_decoder& AudioDecoder::GetDecoder() {
    return this->p_decoder;
}

void AudioDecoder::InitDecoder(const std::string &FilePath) {
	ma_result result = ma_decoder_init_file(FilePath.c_str(), nullptr, &this->p_decoder);
	if (result != MA_SUCCESS) {
		LOG_ERROR("miniaudo -> Error to loading the file:"  + FilePath);
	}
	std::cout << "Audio Decoder -> Init Decoder Completed with file's sample rate: " << p_decoder.outputSampleRate << "Hz"
			  << " and file format: " << p_decoder.outputFormat << std::endl;
	// LOG_INFO("Audio Decoder -> Init Decoder Completed.");
}