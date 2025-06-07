/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
*  File Name: LogSystem.hpp
 *  Lib: Beeplayer I/O Log System definitions
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: I/O, LOG System
 */

#ifndef LOG_HPP
#define LOG_HPP

#include <mutex>

enum LogLevel {
	BP_INFO,
	BP_WARNING,
	BP_ERROR,
	BP_DEBUG
};

class Log {
private:
	Log() = default;
	~Log() = default;
	Log(const Log&) = delete; // Deleted the Copy Constructor 删除拷贝构造函数
	Log& operator=(const Log&) = delete; // Deleted Copy Assignment Operator 删除拷贝赋值运算符

	LogLevel ViewLogLevel = BP_INFO;
	std::mutex LogMutex; // Give it a mutual exclusion 定义一个互斥锁，确保多线程安全

public:
	static Log& GetLogInstance(); // Singleton Pattern 单例模式，只存在一个对象实例

	static void LogOut(const std::string& I_LogMessage, LogLevel LogLevel = BP_INFO);
	static void SetViewLogLevel(LogLevel ViewLogLevel);
};

#define LOG_INFO(LogMessage) Log::GetLogInstance().LogOut(LogMessage, BP_INFO)
#define LOG_WARNING(LogMessage) Log::GetLogInstance().LogOut(LogMessage, BP_WARNING)
#define LOG_ERROR(LogMessage) Log::GetLogInstance().LogOut(LogMessage, BP_ERROR)
#define LOG_DEBUG(LogMessage) Log::GetLogInstance().LogOut(LogMessage, BP_DEBUG)

#endif //LOG_HPP