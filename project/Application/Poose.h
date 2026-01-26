#pragma once
#include "Input.h"
#include "WinApp.h"
#include "Sprite.h"
#include "SpriteCommon.h"
class Poose
{
public:
	Poose();
	~Poose();
	void Initialize();
	void Update();
	void Draw();

private:
	Input* input_;
	WinApp* winApp_;

	SpriteCommon* spriteCommon_;
	Sprite* pooseSprite_;

};

