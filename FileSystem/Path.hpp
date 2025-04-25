/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Path.hpp
 *  Lib: Beeplayer Song List File System
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: FileSystem
 */
#ifndef PATH_HPP
#define PATH_HPP

#include <filesystem>

namespace fs = std::filesystem;

class Path {
	private:
		fs::path p_root_path;
		std::vector<std::string> p_song_names;
		size_t p_current_index = 0;

		// Init The Song List
		void InitSongList();

	public:
		// Constructor
		explicit Path(const std::string& root);

		// Get Next File Path
		std::string NextFilePath();

		// Get Prev File Path
		std::string PrevFilePath();

		// Get Current File Path
		std::string CurrentFilePath() const;

		// Get file name
		static std::string GetFileName(const std::string& path);

		// Get current index
		size_t Index() const { return p_current_index; }

		// Get Song Count
		size_t TotalSong() const { return p_song_names.size(); }

		// Get All Song Names
		void GetAllSongNames() const;

		// Rescan the folder
		void Rescan();
};

#endif //PATH_HPP
