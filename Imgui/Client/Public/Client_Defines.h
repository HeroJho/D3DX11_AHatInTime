#pragma once

#include "Client_Struct.h"
#include <fstream>
#include <iostream>

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_GAMEPLAY , LEVEL_SELECTTOOL, LEVEL_MAPTOOL, LEVEL_CAMTOOL, LEVEL_ANIMTOOL, LEVEL_PARTSTOOL, LEVEL_TESTLEVEL, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
