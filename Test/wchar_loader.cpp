#include "../miniaudio/miniaudio.c"
#include "../miniaudio/miniaudio.h"

#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

// For those who are using windows, but not using utf-8 encoding
// we use the wchar version to load the file
std::vector<std::string> p_song_names;
std::wstring u8tou16(const std::string& u8) {
  std::vector<uint16_t> utf16_buf; // 存储UTF-16代码单元
  for (size_t i = 0; i < u8.size();) {
    uint32_t cp = 0; // Unicode代码点
    uint8_t lead = static_cast<uint8_t>(u8[i]);

    // 解析UTF-8序列
    if (lead < 0x80) {
      cp = lead;
      i += 1;
    } else if ((lead >> 5) == 0x6) {
      cp = (lead & 0x1F) << 6 | (u8[i+1] & 0x3F);
      i += 2;
    } else if ((lead >> 4) == 0xE) {
      cp = (lead & 0x0F) << 12 | (u8[i+1] & 0x3F) << 6 | (u8[i+2] & 0x3F);
      i += 3;
    } else if ((lead >> 3) == 0x1E) {
      cp = (lead & 0x07) << 18 | (u8[i+1] & 0x3F) << 12 | (u8[i+2] & 0x3F) << 6 | (u8[i+3] & 0x3F);
      i += 4;
    } else {
      throw std::runtime_error("无效的UTF-8序列");
    }

    // 转换为UTF-16
    if (cp <= 0xFFFF) {
      utf16_buf.push_back(static_cast<uint16_t>(cp));
    } else {
      cp -= 0x10000;
      utf16_buf.push_back(static_cast<uint16_t>(0xD800 | (cp >> 10)));
      utf16_buf.push_back(static_cast<uint16_t>(0xDC00 | (cp & 0x3FF)));
    }
  }

  // 转换为wchar_t（自动适应平台）
  return std::wstring(utf16_buf.begin(), utf16_buf.end());
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
  ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
  if (pDecoder == NULL) {
    return;
  }

  ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

  (void)pInput;
}

int main() {
  #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
  #endif
  ma_result result;
  ma_decoder decoder;
  ma_device_config deviceConfig;
  ma_device device;

  p_song_names.emplace_back("E:\\Music\\YOASOBI - アドベンチャー.mp3");
  std::wstring u16name = u8tou16(p_song_names[0]);
  result = ma_decoder_init_file_w(u16name.c_str(), NULL, &decoder);
  if (result != MA_SUCCESS) {
    printf("Could not load file: %s\n", p_song_names[0].c_str());
    return -2;
  }

  deviceConfig = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format   = decoder.outputFormat;
  deviceConfig.playback.channels = decoder.outputChannels;
  deviceConfig.sampleRate        = decoder.outputSampleRate;
  deviceConfig.dataCallback      = data_callback;
  deviceConfig.pUserData         = &decoder;

  if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
    printf("Failed to open playback device.\n");
    ma_decoder_uninit(&decoder);
    return -3;
  }

  if (ma_device_start(&device) != MA_SUCCESS) {
    printf("Failed to start playback device.\n");
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    return -4;
  }
  printf("Now playing:%s\n",p_song_names[0].c_str());
  printf("Press Enter to quit...");
  getchar();

  ma_device_uninit(&device);
  ma_decoder_uninit(&decoder);

  return 0;
}