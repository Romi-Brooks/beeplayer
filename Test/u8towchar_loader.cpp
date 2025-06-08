#include <windows.h>
#include <cctype>
#include <cstdio>
#include "../miniaudio/miniaudio.h"
#include "../miniaudio/miniaudio.c"

// 检查字符串是否只包含ASCII字符
bool is_ascii(const char* str) {
    for (; *str; ++str) {
        if (static_cast<unsigned char>(*str) > 127) {
            return false;
        }
    }
    return true;
}

#ifdef _WIN32
// 将UTF-8转换为UTF-16
wchar_t* utf8_to_utf16(const char* utf8_str) {
    if (!utf8_str) return NULL;

    int wchar_count = MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8_str,
        -1,
        NULL,
        0
    );

    if (wchar_count == 0) return NULL;

    wchar_t* wstr = (wchar_t*)malloc(wchar_count * sizeof(wchar_t));
    if (!wstr) return NULL;

    if (MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8_str,
        -1,
        wstr,
        wchar_count) == 0
    ) {
        free(wstr);
        return NULL;
    }
    return wstr;
}
#endif

// 优化后的文件加载函数
ma_result load_audio_file(const char* path, ma_decoder* decoder) {
#ifdef _WIN32
    // 如果是纯ASCII路径，直接使用窄字符API
    if (is_ascii(path)) {
        return ma_decoder_init_file(path, NULL, decoder);
    }

    // 非ASCII路径才进行转换
    wchar_t* wpath = utf8_to_utf16(path);
    if (!wpath) return MA_ERROR;

    ma_result result = ma_decoder_init_file_w(wpath, NULL, decoder);
    free(wpath);
    return result;
#else
    // 其他平台直接使用UTF-8路径
    return ma_decoder_init_file(path, NULL, decoder);
#endif
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}

int main() {
    // 设置控制台输出为UTF-8编码
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    ma_result result;
    ma_device_config deviceConfig;
    ma_device device;
    ma_decoder decoder;

    // // 测试纯ASCII路径
    // const char* ascii_path = "E:\\Music\\english_song.mp3";
    // if (load_audio_file(ascii_path, &decoder) != MA_SUCCESS) {
    //     printf("错误：无法加载音频文件: %s\n", ascii_path);
    // } else {
    //     printf("纯ASCII路径加载成功\n");
    //     ma_decoder_uninit(&decoder);
    // }

    // 测试中文路径
    const char* chinese_path = "E:\\Music\\本兮 - 坏女孩 [AI].mp3";
    if (load_audio_file(chinese_path, &decoder) != MA_SUCCESS) {
        printf("错误：无法加载音频文件: %s\n", chinese_path);
        return -1;
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

    printf("Press Enter to quit...");
    getchar();

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    return 0;
}