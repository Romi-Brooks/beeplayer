/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: AudioDecoder.hpp
 *  Lib: Beeplayer Core engine Audio Decoder lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#ifndef AUDIODECODER_HPP
#define AUDIODECODER_HPP
#include "../miniaudio/miniaudio.h"
#include "string"

class AudioDecoder {
    private:
        ma_decoder p_decoder;

    public:
        AudioDecoder() : p_decoder{} {}

        ma_decoder& GetDecoder();

        void InitDecoder(const std::string& FilePath);

};

#endif //AUDIODECODER_HPP
