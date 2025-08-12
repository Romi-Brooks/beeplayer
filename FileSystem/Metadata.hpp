//
// Created by Romi on 25-7-6.
//

#ifndef METADATA_HPP
#define METADATA_HPP

#include <string>
#include <vector>
#include <stdexcept>

class AudioMetadataReader {
public:
	explicit AudioMetadataReader(const std::string& filePath);

	// 获取歌曲标题
	std::string getSongTitle() const;

	// 获取制作人（优先顺序：PRODUCER > ARTIST > ALBUMARTIST）
	std::string getSongProducer() const;

	// 获取专辑封面（返回二进制数据）
	std::vector<unsigned char> getAlbumCover() const;

private:
	std::string filePath;

	// 辅助函数：检查文件扩展名
	bool isMP3() const;
	bool isWAV() const;

	// WAV文件特有元数据读取
	std::string getWAVTitle() const;
	std::string getWAVProducer() const;
};


#endif //METADATA_HPP
