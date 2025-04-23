/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Decoder.hpp
 *  Lib: Beeplayer Core engine Audio Decoder lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#ifndef DECODER_HPP
#define DECODER_HPP
#include "../miniaudio/miniaudio.h"
#include "string"

class AudioDecoder {
    public:
        AudioDecoder() : p_decoder{} {}

        ma_decoder& GetDecoder();

        void InitDecoder(const std::string& FilePath);

    private:
        ma_decoder p_decoder;
};

#endif //DECODER_HPP
