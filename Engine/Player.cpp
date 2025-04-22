/*  Copyright (c) 2025 Romi Brooks <romi@heyromi.tech>
 *  File Name: Player.cpp
 *  Lib: Beeplayer Core engine Audio Player lib
 *  Author: Romi Brooks
 *  Date: 2025-04-22
 *  Type: Core Engine
 */

#include "Player.hpp"
#include <iostream>

// // Prototype Callback:
// void* Player::Callback(const ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
// 	auto *pDecoder = static_cast<ma_decoder *>(pDevice->pUserData);
// 	if (pDecoder == nullptr) {
// 		std::cout << "Error when load the callback";
// 	}
// 	ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, nullptr);
// 	(void) pInput;
// }

void Player::StaticCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	// 从用户数据获取Player实例
	auto* player = static_cast<Player*>(pDevice->pUserData);
	player->InstanceCallback(pDevice, pOutput, pInput, frameCount);
}

void Player::InstanceCallback(const ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
	auto *pDecoder = static_cast<ma_decoder *>(pDevice->pUserData);
	if (pDecoder == nullptr) {
		std::cout << "Error when load the callback";
	}
	ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, nullptr);
	(void) pInput;
}

void Player::Play(ma_device &Device, ma_decoder &Decoder)  {
	if (ma_device_start(&Device) != MA_SUCCESS) {
		std::cout << "Error when play the file.";
		ma_device_uninit(&Device);
		ma_decoder_uninit(&Decoder);
	}
	std::cout << "Playing...";
}
