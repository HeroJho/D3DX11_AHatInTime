#pragma once

#include <fstream>
#include <iostream>
#include <random>

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	static const float g_fBGMSound = 0.35f;
	static const float g_fEffectSound = 0.4f;
	static const float g_fTalkSound = 0.6f;
	static const float g_fUISound = 1.f;
	static const float g_fSnatSound = 0.6f;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_BOSS, LEVEL_ENDING, LEVEL_END };
	enum SOUND { SOUND_BGM, SOUND_BGM1, SOUND_PLAYER, SOUND_PEFFECT, SOUND_PEFFECT_PUT, SOUND_PEFFECT_SPRT, SOUND_EFFECT, SOUND_SHAKE, SOUND_MEFFECT, SOUND_GETITEM, SOUND_SNAT, SOUND_SNATEFFECT, SOUND_ITEM, SOUND_BOSS1, SOUND_BGMBAG, SOUND_UI, SOUND_END }; // √÷¥Î 32
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;


using namespace Client;
