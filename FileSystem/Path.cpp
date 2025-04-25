/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Path.cpp
 *  Lib: Beeplayer Song List File System
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: FileSystem
 */
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "Path.hpp"


void Path::InitSongList() {
	p_song_names.clear();
	if (!fs::exists(p_root_path) || !fs::is_directory(p_root_path))
		return;

	try {
		auto iterator_opt = fs::directory_options::skip_permission_denied;
		auto iterator_rec = fs::recursive_directory_iterator(p_root_path, iterator_opt);

		for (const auto &file: iterator_rec) {
			try {
				if (!file.is_regular_file())
					continue;

				std::string extension = file.path().extension().string();
				std::ranges::transform(extension.begin(), extension.end(), extension.begin(),
									   [](unsigned char c) { return std::tolower(c); });

				if (extension == ".mp3" || extension == ".wav") {
					p_song_names.push_back(file.path().filename().string());
				}
			} catch (...) {
				// Skip File Error
			}
		}
	} catch (...) {
		// Skip Folder Error
	}
}
Path::Path(const std::string &root) : p_root_path(root) { InitSongList(); }

std::string Path::NextFilePath() {
	if (p_song_names.empty())
		return "";

	p_current_index = (p_current_index + 1) % p_song_names.size();
	return (p_root_path / p_song_names[p_current_index]).string();
}

std::string Path::PrevFilePath() {
	if (p_song_names.empty())
		return "";

	p_current_index = (p_current_index - 1 + p_song_names.size()) % p_song_names.size();
	return (p_root_path / p_song_names[p_current_index]).string();
}

std::string Path::CurrentFilePath() const {
	if (p_song_names.empty())
		return "";
	return (p_root_path / p_song_names[p_current_index]).string();
}

std::string Path::GetFileName(const std::string &path) { return fs::path(path).filename().string(); }

void Path::GetAllSongNames() const {
	for(const auto& it : p_song_names) {
		std::cout << it << std::endl;
	}
}

void Path::Rescan() {
	InitSongList();
	p_current_index = 0;
}



