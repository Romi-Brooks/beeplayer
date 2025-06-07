/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
*  File Name: Status.hpp
 *  Lib: Beeplayer Core engine Progress Status definitions
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: Status, Core Engine
 */

#ifndef STATUS_HPP
#define STATUS_HPP

// Basic Lib
#include "../miniaudio/miniaudio.h"
#include "Buffering.hpp"
#include "Decoder.hpp"

class Status {
public:
	explicit Status(AudioDecoder& Decoder);

	// void ProgressThread(AudioDecoder& Decoder, AudioBuffering& Buffer);

	void SetFileLength(AudioDecoder &Decoder);
        void ProgressThread(AudioDecoder &Decoder, AudioBuffering &Buffer) const;
        ma_uint64 GetTotalFrames() const  { return p_fileTotalFrames; }
	void ResetStatus();

private:
	std::atomic<ma_uint64> p_fileTotalFrames{0};
};



#endif //STATUS_HPP