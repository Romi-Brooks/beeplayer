/*
 *  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *
 *  Beeplayer non-Release Version
 *  A lightweight and cross-platform music player based on miniaudio and C++.
 *
 *  Thanks to David Reid provided us a such powerful and useful lib "miniaudio"
 *
 *  All of the code is not the final version now,
 *  and it should be logically rewritten once I have implemented the basic functions.
 *
 *  Thanks to music make the world so beautiful. :)
 */

#include <iostream>
#include <cstring>

#include "Engine/Buffering.hpp"
#include "Engine/DataCallback.hpp"
#include "Engine/Decoder.hpp"
#include "Engine/Device.hpp"
#include "Engine/Player.hpp"
#include "Engine/Status.hpp"
#include "FileSystem/Path.hpp"
#include "Log/LogSystem.hpp"
#include "miniaudio/miniaudio.h"

// To provide the Listen Event can be Non-blocking.
#ifdef _WIN32 // For windows platform Non-blocking input
#include <windows.h>
#include <conio.h>
bool IsInputAvailable() {
    return _kbhit() != 0;
}
#else
#include <sys/select.h>
bool IsInputAvailable() {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    timeval timeout = {0, 0};
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout) > 0;
}
#endif
void ListenEvent(Path& Pather, AudioPlayer& Player, AudioDevice& Device, AudioDecoder& Decoder, Status& Timer, AudioBuffering& Buffer) {
    std::cout << "Press e to Exit, p to Pause, n to next, u to Prev." << std::endl;
    while (true) {
    	std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 关键修改：添加休眠
    	// Player.NextFileCheck(Buffer, Timer, Pather, Decoder, Device, data_callback); // Check if the current song is over
    	// 非阻塞检查用户输入
        if (IsInputAvailable()) {
            char key;
            std::cin >> key;
            switch (tolower(key)) {
                case 'e': {
                    Player.Exit(Device,Decoder);
                    exit(0);
                }
                case 'p': {
                    if (ma_device_is_started(&Device.GetDevice())) {
                        ma_device_stop(&Device.GetDevice());
                        std::cout << "Paused." << std::endl;
                    } else {
                        ma_device_start(&Device.GetDevice());
                        std::cout << "Resumed." << std::endl;
                    }
                    break;
                }
                case 'n': {
                    Player.Switch(Pather, Decoder, Device, data_callback, Timer, Buffer, SwitchAction::NEXT);
                	LOG_INFO("Next Song!");
                	ListenEvent(Pather, Player, Device, Decoder, Timer, Buffer);
                    break;
                }
                case 'u': {
                	Player.Switch(Pather, Decoder, Device, data_callback, Timer, Buffer, SwitchAction::PREV);
                	LOG_INFO("Prev Song!");
                	ListenEvent(Pather, Player, Device, Decoder, Timer, Buffer);
                    break;
                }
                default: {
                	LOG_WARNING("Unknown Command.");
                    break;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
	#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
		LOG_WARNING("WinMarco -> Set the code page to CP_UTF8");
	#endif
	std::string rootPath;
	if (argc == 3) {
		if (std::string(argv[1]) == "-root") {
			rootPath = argv[2];
		} else {
			LOG_WARNING("Using: -root \"Path/to/music\" to set the root path.");
			return 1;
		}
	} else {
		// 交互式获取路径
		std::cout << "Input the root path: ";
		std::getline(std::cin, rootPath);

		// 确保输入非空
		while (rootPath.empty()) {
			LOG_WARNING("The root path can not be empty.");
			std::cout << "Input the root path: ";
			std::getline(std::cin, rootPath);
		}
	}

	// 创建Path对象
	Path Pather(rootPath);

	AudioDecoder Decoder;
	AudioDevice& Device = AudioDevice::GetDeviceInstance();
	AudioPlayer Player;

	Player.InitDecoder(Pather, Decoder); // 总是在主函数中先调用，才能够给双缓冲实例使用
	AudioBuffering DoubleBuffering(&Decoder.GetDecoder());	// 创建双缓冲实例并关联到设备

	Player.InitDevice(Decoder, Device, data_callback, DoubleBuffering); // 确保双缓冲被正确初始化，才能给到回调函数来获取信息
	Status Timer(Decoder);

        std::thread(&Status::ProgressThread, &Timer, std::ref(Decoder), std::ref(DoubleBuffering)).detach(); // Start the Time Counter Thread
	std::thread(&AudioPlayer::NextFileCheck, &Player, std::ref(DoubleBuffering), std::ref(Timer), std::ref(Pather), std::ref(Decoder), std::ref(Device), data_callback).detach();
	std::thread(&AudioPlayer::Play, &Player, std::ref(Device),std::ref(Decoder)).detach();

	ListenEvent(Pather, Player, Device, Decoder, Timer, DoubleBuffering);

	std::cin.get();
	return 0;
}

