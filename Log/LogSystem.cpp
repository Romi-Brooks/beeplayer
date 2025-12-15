/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: LogSystem.cpp
 *  Lib: Beeplayer I/O Log System
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: I/O, LOG System
 */

#include "LogSystem.hpp"

// Standard Lib
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>


// Forward Function
std::string Log::GetLogLevelName(const LogLevel Level) {
	switch (Level) {
		case BP_INFO: return "INFO";
		case BP_WARNING: return "WARNING";
		case BP_ERROR: return "ERROR";
		case BP_DEBUG: return "DEBUG";
		default: return "Unknown Level";
	}
}

std::string Log::GetLogChannelName(const LogChannel Channel) {
	switch (Channel) {
		case CH_MINIAUDIO: return "Miniaudio";
		case CH_BUFFERING: return  "Buffering";
		case CH_DECODER: return "Decoder";
		case CH_DEVICE: return "Device";
		case CH_CONTROLLER: return "Controller";
		case CH_PLAYER: return "Player";
		case CH_STATUS: return "Status";
		case CH_ENCODING: return "Encoding";
             case CH_METADATA: return "Metadata";
		case CH_PATH: return "Pather";
             case CH_QT: return "QT";
		case CH_LOG: return "Logger";
		case CH_DEBUG: return "Debugger";
		default: return "Unknown Channel";
	}
}

std::string Log::GetCurrentLogTime() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	// 线程安全的时间转换
	struct tm tm_buf{};
#if defined(_WIN32)
	localtime_s(&tm_buf, &in_time_t);  // Windows
#else
	localtime_r(&in_time_t, &tm_buf);  // Linux/MacOS
#endif

	// 添加毫秒精度
	auto since_epoch = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch).count() % 1000;

	char buffer[80];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_buf);

	return std::string(buffer) + '.' + std::to_string(millis);
}

Log& Log::GetLogInstance() {
    static Log LogInstance;
    return LogInstance;
}

void Log::SetViewLogLevel(LogLevel Level) {
	LogOut(LogLevel::BP_WARNING, LogChannel::CH_LOG, "Set The Log Level to ", GetLogLevelName(Level));
    GetLogInstance().ViewLogLevel = Level;
}

