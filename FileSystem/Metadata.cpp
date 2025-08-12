#include "Metadata.hpp"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/toolkit/tpropertymap.h>
#include <taglib/mpeg/mpegfile.h>
#include <taglib/riff/wav/wavfile.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/id3v2/frames/attachedpictureframe.h>
#include <taglib/riff/wav/infotag.h>  // WAV 的 RIFF INFO 标签
#include <cctype> // std::tolower

AudioMetadataReader::AudioMetadataReader(const std::string& filePath)
    : filePath(filePath) {}

bool AudioMetadataReader::isMP3() const {
    if (filePath.length() < 4) return false;
    std::string ext = filePath.substr(filePath.length() - 4);
    for (char& c : ext) c = std::tolower(c);
    return ext == ".mp3";
}

bool AudioMetadataReader::isWAV() const {
    if (filePath.length() < 4) return false;
    std::string ext = filePath.substr(filePath.length() - 4);
    for (char& c : ext) c = std::tolower(c);
    return ext == ".wav";
}

std::string AudioMetadataReader::getSongTitle() const {
    if (isWAV()) {
        return getWAVTitle();
    }

    TagLib::FileRef file(filePath.c_str());
    if (file.isNull() || !file.tag()) {
        throw std::runtime_error("无法打开文件或读取标签");
    }
    return file.tag()->title().to8Bit(true);
}

std::string AudioMetadataReader::getSongProducer() const {
    if (isWAV()) {
        return getWAVProducer();
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

std::vector<unsigned char> AudioMetadataReader::getAlbumCover() const {
    std::vector<unsigned char> imageData;

    // MP3 封面处理
    if (isMP3()) {
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
    else if (isWAV()) {
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
std::string AudioMetadataReader::getWAVTitle() const {
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

std::string AudioMetadataReader::getWAVProducer() const {
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