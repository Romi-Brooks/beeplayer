/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: LogSystem.hpp
 *  Lib: Beeplayer I/O Log System
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: I/O LOG
 */

#ifndef LOG_HPP
#define LOG_HPP

#include <mutex>

enum LogLevel {
	INFO,
	WARNING,
	ERROR,
	DEBUG
};

class Log {
private:
	Log() = default;
	~Log() = default;
	Log(const Log&) = delete; // Deleted the Copy Constructor 删除拷贝构造函数
	Log& operator=(const Log&) = delete; // Deleted Copy Assignment Operator 删除拷贝赋值运算符

	LogLevel ViewLogLevel = INFO;
	std::mutex LogMutex; // Give it a mutual exclusion 定义一个互斥锁，确保多线程安全

public:
	static Log& GetLogInstance(); // Singleton Pattern 单例模式，只存在一个对象实例

	static void LogOut(const std::string& I_LogMessage, LogLevel LogLevel = INFO);
	static void SetViewLogLevel(LogLevel ViewLogLevel);
};

#define LOG_INFO(LogMessage) Log::GetLogInstance().LogOut(LogMessage, INFO)
#define LOG_WARNING(LogMessage) Log::GetLogInstance().LogOut(LogMessage, WARNING)
#define LOG_ERROR(LogMessage) Log::GetLogInstance().LogOut(LogMessage, ERROR)
#define LOG_DEBUG(LogMessage) Log::GetLogInstance().LogOut(LogMessage, DEBUG)

#endif //LOG_HPP