#include "Application/FramWork/MyGame.h"
#pragma comment(lib,"dxcompiler.lib")

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	MyGame game;
	game.Run();
	return 0;
}