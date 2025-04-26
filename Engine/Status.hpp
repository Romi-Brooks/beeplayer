/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Status.hpp
 *  Lib: Beeplayer Core engine Progress Status
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: Core Engine
 */

#ifndef STATUS_HPP
#define STATUS_HPP

#include "../miniaudio/miniaudio.h"
#include "../Log/LogSystem.hpp"
#include "Buffering.hpp"
#include "Decoder.hpp"

class Status {
public:
	explicit Status(AudioDecoder& Decoder);

	void ProgressThread(AudioDecoder& Decoder, AudioBuffering& Buffer);

	void SetFileLength(AudioDecoder& Decoder);
	ma_uint64 GetTotalFrames() const  { return p_fileTotalFrames; }
	void ResetStatus();
private:
	std::atomic<ma_uint64> p_fileTotalFrames{0};
};



#endif //STATUS_HPP
