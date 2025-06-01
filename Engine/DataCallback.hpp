/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: DataCallback.cpp
 *  Lib: Beeplayer Callback Function definitions for miniaudio
 *  Author: Romi Brooks
 *  Date: 2025-06-1
 *  Type: Functions, Core Engine
 */

#ifndef DATACALLBACK_HPP
#define DATACALLBACK_HPP

// Basic Lib
#include "../miniaudio/miniaudio.h"

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

#endif //DATACALLBACK_HPP
