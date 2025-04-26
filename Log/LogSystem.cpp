/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: LogSystem.cpp
 *  Lib: Beeplayer I/O Log System
 *  Author: Romi Brooks
 *  Date: 2025-04-24
 *  Type: I/O LOG
 */

#include <string>
#include <iostream>
#include <mutex>
#include <chrono>

#include "LogSystem.hpp"

// Foward Function
std::string GetLogLevel(const LogLevel& LogLevel) {
    std::string LevelStr;
    switch (LogLevel) {
        case INFO: LevelStr = "INFO"; break;
        case WARNING: LevelStr = "WARNING"; break;
        case ERROR: LevelStr = "ERROR"; break;
        case DEBUG: LevelStr = "DEBUG"; break;
    }
    return LevelStr;
}
auto GetCurrentTime() {
    /* After You review this code:
     * std::chrono: 是 C++ 中用于处理时间的库, 提供了各种时间相关的功能
     * system_clock: 是 std::chrono 库中的一种时钟, 表示系统时间(即计算机的操作系统时间)
     * now(): 是 system_clock 的一个静态成员函数, 用于获取当前时间点
     * time_point: 是 std::chrono 库中表示时间点的类型, 表示从某个 epoch(通常是指 1970 年 1 月 1 日 00:00:00 UTC)开始的时间点 */

    /* The Function: std::chrono::system_clock::now()
     * 是 C++ 标准库中用于获取当前时间的函数。
     * 它返回一个 std::chrono::system_clock::time_point 类型的对象 */
    const auto Time = std::chrono::system_clock::now();

    /* The Function: std::chrono::system_clock::to_time_t(now)
     * 是将 std::chrono::system_clock::time_point 类型的时间点转换为 time_t 类型
     * time_t 是 C 和 C++ 中表示时间的类型, 通常是一个整数, 表示从 1970 年 1 月 1 日 00:00:00 UTC 开始的秒数(即 Unix 时间戳) */
    auto TimeT = std::chrono::system_clock::to_time_t(Time);

    /* std::stringstream: 是 C++ 中用于字符串流操作的类, 允许像操作 I/O 流一样操作字符串
     * std::put_time: 是一个格式化时间的函数, 用于将时间格式化为字符串
     * std::localtime(&TimeT): 是 C 标准库中的函数, 用于将 time_t 类型的时间转换为本地时间的 tm 结构 */
    std::stringstream TimeString;
    TimeString << std::put_time(std::localtime(&TimeT), "%Y-%m-%d %X");
    return TimeString;
}

Log& Log::GetLogInstance() {
    static Log LogInstance;
    return LogInstance;
}

void Log::LogOut(const std::string& I_LogMessage, LogLevel I_Level) {
    std::lock_guard<std::mutex> lock(GetLogInstance().LogMutex);

    std::string FullLogMessage = "[" + GetCurrentTime().str() + "] [" + GetLogLevel(I_Level) + "] " + I_LogMessage;
    std::cout << FullLogMessage << std::endl;
}

void Log::SetViewLogLevel(LogLevel I_Level) {
    GetLogInstance().ViewLogLevel = I_Level;
}