/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: Encoding.cpp
 *  Lib: Beeplayer Song List File System
 *  Author: Romi Brooks
 *  Date: 2025-06-08
 *  Type: FileEncoding FileSystem
 */

// Standard Lib
#include <vector>

// Basic Lib
#include "Encoding.hpp"
#include "../Log/LogSystem.hpp"

bool Encoding::IsPureAscii(const std::string &FileName) {
  for (char c : FileName) {
    // 检查字符是否为ASCII（0-127）
    if (!isascii(static_cast<unsigned char>(c))) {
      return false;
    }
  }
  return true;
}
std::wstring Encoding::u8tou16(const std::string &u8)  {
  std::vector<uint16_t> utf16_buf; // 存储UTF-16代码单元
  for (size_t i = 0; i < u8.size();) {
    uint32_t cp = 0; // Unicode代码点
    auto lead = static_cast<uint8_t>(u8[i]);

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
      LOG_ERROR("Encoding -> Error UTF-8 Sequence.");
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
  LOG_WARNING("Encoding -> Converted the File Name from u8 to u16!");
  // 转换为wchar_t（自动适应平台）
  return std::wstring(utf16_buf.begin(), utf16_buf.end());
}