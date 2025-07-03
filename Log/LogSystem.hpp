/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: LogSystem.hpp
 *  Lib: Beeplayer I/O Log System definitions
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: I/O, LOG System
 */

#ifndef LOG_HPP
#define LOG_HPP

// Standard Lib
#include <mutex>
#include <sstream>
#include <iostream>
#include <string>
#include <chrono>

enum LogLevel {
	BP_INFO,
	BP_WARNING,
	BP_ERROR,
	BP_DEBUG
};

enum LogChannel {
	CH_MINIAUDIO,
	CH_BUFFERING,
	CH_DECODER,
	CH_DEVICE,
	CH_CONTROLLER,
	CH_PLAYER,
	CH_STATUS,
	CH_ENCODING,
	CH_PATH,
	CH_IMGUI,
	CH_GLFW,
	CH_LOG,
	CH_DEBUG
};

class Log {
private:
	Log() = default;
	~Log() = default;

	LogLevel ViewLogLevel = BP_INFO;

	static std::string GetLogLevelName(LogLevel Level);
	static std::string GetLogChannelName(LogChannel Channel);

	static std::string GetCurrentLogTime();

	std::mutex LogMutex; // Give it a mutual exclusion 定义一个互斥锁，确保多线程安全

public:
	Log(const Log &) = delete; // Deleted the Copy Constructor 删除拷贝构造函数
	Log &operator=(const Log &) = delete; // Deleted Copy Assignment Operator 删除拷贝赋值运算符
	static Log &GetLogInstance(); // Singleton Pattern 单例模式，只存在一个对象实例

	template<typename... Args>
	static void LogOut(const LogLevel Level = BP_INFO, const LogChannel Channel = CH_DEBUG, Args... Msg) {
		std::lock_guard<std::mutex> lock(GetLogInstance().LogMutex);

		if (Level >= Log::GetLogInstance().ViewLogLevel) {

			// 直接使用字符串函数
			std::cout << "[" << GetCurrentLogTime() << "] "
					  << "[" << GetLogLevelName(Level) << "] "
					  << GetLogChannelName(Channel) << " -> ";

			// 折叠表达式直接输出
			(std::cout << ... << std::forward<Args>(Msg));

			std::cout << std::endl;
		}

	}

	static void SetViewLogLevel(LogLevel Level);
};
#endif //LOG_HPP