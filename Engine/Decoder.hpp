/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Decoder.hpp
 *  Lib: Beeplayer Core engine Audio Decoder lib definitions
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Decoder, Core Engine
 */

#ifndef DECODER_HPP
#define DECODER_HPP

// Standard Lib
#include <string>

// Basic Lib
#include "../miniaudio/miniaudio.h"

class AudioDecoder {
    public:
        AudioDecoder() : p_decoder{} {}

        ma_decoder& GetDecoder();

        void InitDecoder(const std::string& FilePath);

    private:
        ma_decoder p_decoder;
};

#endif //DECODER_HPP
