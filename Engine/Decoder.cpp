/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
*  File Name: Decoder.cpp
 *  Lib: Beeplayer Core engine Audio Decoder lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Decoder, Core Engine
 */
#include "Decoder.hpp"

// Standard Lib
#include <sstream>

// Basic Lib
#include "../Log/LogSystem.hpp"

ma_decoder& AudioDecoder::GetDecoder() {
	return this->p_decoder;
}

void AudioDecoder::InitDecoder(const std::string &FilePath) {
	ma_result result = ma_decoder_init_file(FilePath.c_str(), nullptr, &this->p_decoder);
	if (result != MA_SUCCESS) {
		LOG_ERROR("miniaudo -> Error to loading the file:"  + FilePath);
	}
	std::stringstream ss;
	ss << "Audio Decoder -> Init Decoder Completed with file's sample rate: " << p_decoder.outputSampleRate << "Hz"
			  << " and file format: " << p_decoder.outputFormat;
	LOG_INFO(ss.str());
}