/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Metadata.hpp
 *  Lib: Beeplayer Metadata getter definitions
 *  Author: Romi Brooks
 *  Date: 2025-07-10
 *  Type: FileSystem, Metadata
 */

#ifndef METADATA_HPP
#define METADATA_HPP

#include <string>
#include <vector>

class AudioMetadataReader {
public:
    AudioMetadataReader(const AudioMetadataReader&) = delete;
    AudioMetadataReader& operator=(const AudioMetadataReader&) = delete;

    static AudioMetadataReader& getInstance() {
        static AudioMetadataReader instance;
        return instance;
    }

    // 添加宽字符串版本的重载
    std::string getSongTitle(const std::string& filePath) const;
    std::string getSongTitle(const std::wstring& filePath) const;  // 新增宽字符版本

    std::string getSongProducer(const std::string& filePath) const;
    std::string getSongProducer(const std::wstring& filePath) const;  // 新增宽字符版本

    std::vector<unsigned char> getAlbumCover(const std::string& filePath) const;
    std::vector<unsigned char> getAlbumCover(const std::wstring& filePath) const;  // 新增宽字符版本

private:
    AudioMetadataReader() = default;

    // 内部实现函数添加宽字符版本
    bool isMP3(const std::string& filePath) const;
    bool isMP3(const std::wstring& filePath) const;  // 新增

    bool isWAV(const std::string& filePath) const;
    bool isWAV(const std::wstring& filePath) const;  // 新增

    std::string getWAVTitle(const std::string& filePath) const;
    std::string getWAVTitle(const std::wstring& filePath) const;  // 新增

    std::string getWAVProducer(const std::string& filePath) const;
    std::string getWAVProducer(const std::wstring& filePath) const;  // 新增
};

#endif //METADATA_HPP
