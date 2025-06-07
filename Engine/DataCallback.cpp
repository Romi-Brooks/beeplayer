/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: DataCallback.cpp
 *  Lib: Beeplayer Callback Function for miniaudio
 *  Author: Romi Brooks
 *  Date: 2025-06-1
 *  Type: Functions, Core Engine
 */
#include "DataCallback.hpp"

// Standard Lib
#include <cstring>

// Basic Lib
#include "../miniaudio/miniaudio.h"
#include "Buffering.hpp"
#include "Decoder.hpp"

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
  auto* buffering = static_cast<AudioBuffering*>(pDevice->pUserData);
  static ma_uint64 consumedFrames = 0;

  const int currentBufIdx = buffering->GetActiveBuffer();
  AudioBuffering::Buffer& currentBuf = buffering->GetBuffers()[currentBufIdx];

  if (!currentBuf.s_ready) {
    memset(pOutput, 0, frameCount * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels));
    return;
  }

  const ma_uint64 availableFrames = currentBuf.s_totalFrames - consumedFrames;
  const ma_uint64 framesToCopy = std::min(static_cast<ma_uint64>(frameCount), availableFrames);

  const size_t bytesToCopy = framesToCopy * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
  memcpy(
          pOutput,
          currentBuf.s_data.data() + consumedFrames * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels),
          bytesToCopy
  );

  consumedFrames += framesToCopy;
  buffering->ConsumeFrames(framesToCopy);

  if (consumedFrames >= currentBuf.s_totalFrames) {
    currentBuf.s_ready = false;
    buffering->SwitchBuffer();
    consumedFrames = 0;
  }

  if (framesToCopy < frameCount) {
    const size_t remainingBytes = (frameCount - framesToCopy) * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels);
    memset(static_cast<char*>(pOutput) + bytesToCopy, 0, remainingBytes);
  }
}