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
#include "../FileSystem/Encoding.hpp"
#include "../Log/LogSystem.hpp"

ma_decoder& AudioDecoder::GetDecoder() {
	return this->p_decoder;
}

void AudioDecoder::InitDecoder(const std::string &FilePath) {
	ma_result result;
	#ifdef _WIN32
		if (Encoding::IsPureAscii(FilePath)) {
			result = ma_decoder_init_file(FilePath.c_str(), nullptr, &this->p_decoder);
		} else {
			std::wstring widePath = Encoding::u8tou16(FilePath);
			result = ma_decoder_init_file_w(widePath.c_str(), nullptr, &this->p_decoder);
		}
	#else
		result = ma_decoder_init_file(FilePath.c_str(), nullptr, &this->p_decoder);
	#endif

	if (result != MA_SUCCESS) {
		LOG_ERROR("miniaudio -> Error loading file: " + FilePath);
		return;
	}
	std::stringstream ss;
	ss << "Audio Decoder -> Init completed. Sample rate: "
	   << p_decoder.outputSampleRate << "Hz, Format: "
	   << p_decoder.outputFormat;
	LOG_INFO(ss.str());
}