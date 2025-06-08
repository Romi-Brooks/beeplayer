/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Encoding.hpp
 *  Lib: Beeplayer Song List File System
 *  Author: Romi Brooks
 *  Date: 2025-06-08
 *  Type: FileEncoding FileSystem
 */

// We need to know that in Windows, there are many encoding formats like:
// UTF-8 (normally used now), or GBK (In some Chinese's Windows os, the terminal use this encoding), etc.
// These diverse encoding formats cause many problems. For example, when you try to play a Chinese or Japanese song,
// As you can imagine, if your file's encoding is UTF-8 and your application is also UTF-8,
// but your terminal uses GBK encoding. it cannot display the correct content. this can lead to the "mojibake" problem.
// The output being garbled is probably the least of the issues.
// When you try to load a file using the UTF-8 encoding format,
// but the load function does not process the open logic correctly
// (like if your ANSI code page is GBK, which does not include all the characters in UTF-8)
// It can cause loading issues that are hard to resolve.

// What we should do is:
// When we detect that the user is on Windows and the playlist contains non-English songs,
// we should convert the UTF-8 encoding to the wide char encoding (aka UTF-16 in Windows).
// Then use the "ma_decoder_init_file_w()" function to load the file.
// This should solve the loading problems.
// Furthermore, we can use macro compilation and the Windows API SetConsoleOutputCP(CP_UTF8) to set the ANSI code page's
// encoding to UTF-8, and output the untranslated file name (utf-8) to the terminal directly.
// This should work well.

#ifndef ENCODING_HPP
#define ENCODING_HPP

// Standard Lib
#include <string>

class Encoding {
    public:

    static bool IsPureAscii(const std::string& FileName);

    static std::wstring u8tou16(const std::string& u8);
};



#endif //ENCODING_HPP
