//
// Created by Romi on 25-4-22.
//

#ifndef AUDIODECODER_HPP
#define AUDIODECODER_HPP
#include "../miniaudio/miniaudio.h"
#include "string"
#include <iostream>

class AudioDecoder {
    private:
        ma_decoder p_decoder;

    public:
        AudioDecoder() : p_decoder{} {}

        ma_decoder& GetDecoder(){
            return this->p_decoder;
        }

        void InitDecoder(const std::string& FilePath) {
            ma_result result = ma_decoder_init_file(FilePath.c_str(), nullptr, &this->p_decoder);
            if (result != MA_SUCCESS) {
                std::cout << "Error to loading the file:" << FilePath << std::endl;
            }
            // std::cout << "Init Decoder Completed." << std::endl;
        }



};



#endif //AUDIODECODER_HPP
