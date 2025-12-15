/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Metadata.cpp
 *  Lib: Beeplayer Metadata getter
 *  Author: Romi Brooks
 *  Date: 2025-07-10
 *  Type: FileSystem, Metadata
 */

#include "Metadata.hpp"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include <taglib/mpegfile.h>
#include <taglib/wavfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/infotag.h>  // WAV 的 RIFF INFO 标签
#include <cctype> // std::tolower
#include <locale>
#include <codecvt>

// u8str
// 辅助函数：检查文件扩展名
bool AudioMetadataReader::isMP3(const std::string& filePath) const {
    if (filePath.length() < 4) return false;
    std::string ext = filePath.substr(filePath.length() - 4);
    for (char& c : ext) c = std::tolower(c);
    return ext == ".mp3";
}

bool AudioMetadataReader::isWAV(const std::string& filePath) const {
    if (filePath.length() < 4) return false;
    std::string ext = filePath.substr(filePath.length() - 4);
    for (char& c : ext) c = std::tolower(c);
    return ext == ".wav";
}

std::string AudioMetadataReader::getSongTitle(const std::string& filePath) const {
    if (isWAV(filePath)) {
        return getWAVTitle(filePath);
    }

    TagLib::FileRef file(filePath.c_str());
    if (file.isNull() || !file.tag()) {
        throw std::runtime_error("无法打开文件或读取标签");
    }
    return file.tag()->title().to8Bit(true);
}

std::string AudioMetadataReader::getSongProducer(const std::string& filePath) const {
    if (isWAV(filePath)) {
        return getWAVProducer(filePath);
    }

    TagLib::FileRef file(filePath.c_str());
    if (file.isNull() || !file.tag()) {
        throw std::runtime_error("无法打开文件或读取标签");
    }

    // MP3 文件处理
    const TagLib::PropertyMap& properties = file.file()->properties();
    if (properties.contains("PRODUCER"))
        return properties["PRODUCER"].front().to8Bit(true);
    if (properties.contains("ARTIST"))
        return properties["ARTIST"].front().to8Bit(true);
    if (properties.contains("ALBUMARTIST"))
        return properties["ALBUMARTIST"].front().to8Bit(true);

    return "";
}

std::vector<unsigned char> AudioMetadataReader::getAlbumCover(const std::string& filePath) const {
    std::vector<unsigned char> imageData;

    // MP3 封面处理
    if (isMP3(filePath)) {
        TagLib::MPEG::File file(filePath.c_str());
        if (file.ID3v2Tag()) {
            TagLib::ID3v2::FrameList frames = file.ID3v2Tag()->frameList("APIC");
            if (!frames.isEmpty()) {
                auto* cover = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
                if (cover) {
                    const TagLib::ByteVector& data = cover->picture();
                    imageData.assign(data.begin(), data.end());
                }
            }
        }
    }
    // WAV 封面处理 (ID3v2标签)
    else if (isWAV(filePath)) {
        TagLib::RIFF::WAV::File file(filePath.c_str());
        if (file.ID3v2Tag()) {
            TagLib::ID3v2::FrameList frames = file.ID3v2Tag()->frameList("APIC");
            if (!frames.isEmpty()) {
                auto* cover = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
                if (cover) {
                    const TagLib::ByteVector& data = cover->picture();
                    imageData.assign(data.begin(), data.end());
                }
            }
        }
    }

    return imageData;
}

// WAV 文件元数据处理
std::string AudioMetadataReader::getWAVTitle(const std::string& filePath) const {
    TagLib::RIFF::WAV::File file(filePath.c_str());

    // 优先尝试 ID3v2 标签
    if (file.ID3v2Tag() && !file.ID3v2Tag()->title().isEmpty()) {
        return file.ID3v2Tag()->title().to8Bit(true);
    }

    // 尝试 RIFF INFO 标签
    if (file.InfoTag() && !file.InfoTag()->title().isEmpty()) {
        return file.InfoTag()->title().to8Bit(true);
    }

    return "";
}

std::string AudioMetadataReader::getWAVProducer(const std::string& filePath) const {
    TagLib::RIFF::WAV::File file(filePath.c_str());

    // 优先尝试 ID3v2 标签
    if (file.ID3v2Tag()) {
        if (!file.ID3v2Tag()->artist().isEmpty())
            return file.ID3v2Tag()->artist().to8Bit(true);
    }

    // 尝试 RIFF INFO 标签
    if (file.InfoTag()) {
        // WAV 文件中制作人可能存储在 artist 字段
        if (!file.InfoTag()->artist().isEmpty())
            return file.InfoTag()->artist().to8Bit(true);

        // 或存储在 comment 字段
        if (!file.InfoTag()->comment().isEmpty())
            return file.InfoTag()->comment().to8Bit(true);
    }

    return "";
}


// u16 wchar str
// 辅助函数：检查文件扩展名
bool AudioMetadataReader::isMP3(const std::wstring& filePath) const {
    // 转换为UTF-8窄字符串调用窄版本
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return isMP3(converter.to_bytes(filePath));
}

bool AudioMetadataReader::isWAV(const std::wstring& filePath) const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return isWAV(converter.to_bytes(filePath));
}

std::string AudioMetadataReader::getSongTitle(const std::wstring& filePath) const {
    if (isWAV(filePath)) {
        return getWAVTitle(filePath);
    }

    TagLib::FileRef file(filePath.c_str());  // TagLib支持宽字符路径
    if (file.isNull() || !file.tag()) {
        throw std::runtime_error("无法打开文件或读取标签");
    }
    return file.tag()->title().to8Bit(true);
}

std::string AudioMetadataReader::getSongProducer(const std::wstring& filePath) const {
    if (isWAV(filePath)) {
        return getWAVProducer(filePath);
    }

    TagLib::FileRef file(filePath.c_str());
    if (file.isNull() || !file.tag()) {
        throw std::runtime_error("无法打开文件或读取标签");
    }

    const TagLib::PropertyMap& properties = file.file()->properties();
    if (properties.contains("PRODUCER"))
        return properties["PRODUCER"].front().to8Bit(true);
    if (properties.contains("ARTIST"))
        return properties["ARTIST"].front().to8Bit(true);
    if (properties.contains("ALBUMARTIST"))
        return properties["ALBUMARTIST"].front().to8Bit(true);

    return "";
}

std::vector<unsigned char> AudioMetadataReader::getAlbumCover(const std::wstring& filePath) const {
    std::vector<unsigned char> imageData;

    if (isMP3(filePath)) {
        TagLib::MPEG::File file(filePath.c_str());
        if (file.ID3v2Tag()) {
            TagLib::ID3v2::FrameList frames = file.ID3v2Tag()->frameList("APIC");
            if (!frames.isEmpty()) {
                auto* cover = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
                if (cover) {
                    const TagLib::ByteVector& data = cover->picture();
                    imageData.assign(data.begin(), data.end());
                }
            }
        }
    }
    else if (isWAV(filePath)) {
        TagLib::RIFF::WAV::File file(filePath.c_str());
        if (file.ID3v2Tag()) {
            TagLib::ID3v2::FrameList frames = file.ID3v2Tag()->frameList("APIC");
            if (!frames.isEmpty()) {
                auto* cover = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
                if (cover) {
                    const TagLib::ByteVector& data = cover->picture();
                    imageData.assign(data.begin(), data.end());
                }
            }
        }
    }

    return imageData;
}

// WAV 文件元数据处理
std::string AudioMetadataReader::getWAVTitle(const std::wstring& filePath) const {
    TagLib::RIFF::WAV::File file(filePath.c_str());
    // 优先尝试 ID3v2 标签
    if (file.ID3v2Tag() && !file.ID3v2Tag()->title().isEmpty()) {
        return file.ID3v2Tag()->title().to8Bit(true);
    }

    // 尝试 RIFF INFO 标签
    if (file.InfoTag() && !file.InfoTag()->title().isEmpty()) {
        return file.InfoTag()->title().to8Bit(true);
    }

    return "";
}

std::string AudioMetadataReader::getWAVProducer(const std::wstring& filePath) const {
    TagLib::RIFF::WAV::File file(filePath.c_str());
    // 优先尝试 ID3v2 标签
    if (file.ID3v2Tag()) {
        if (!file.ID3v2Tag()->artist().isEmpty())
            return file.ID3v2Tag()->artist().to8Bit(true);
    }

    // 尝试 RIFF INFO 标签
    if (file.InfoTag()) {
        // WAV 文件中制作人可能存储在 artist 字段
        if (!file.InfoTag()->artist().isEmpty())
            return file.InfoTag()->artist().to8Bit(true);

        // 或存储在 comment 字段
        if (!file.InfoTag()->comment().isEmpty())
            return file.InfoTag()->comment().to8Bit(true);
    }

    return "";
}
