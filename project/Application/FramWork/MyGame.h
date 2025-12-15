#pragma once
#include "Framework.h"
#include <vector>
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "Camera.h"
#include "Player.h"
#include "Enemy.h"
#include "ParticleManager.h"
#include "RailCamera.h"
#include "Fade.h"

class MyGame : public Framework {
public:
	// Framework の仮想関数を実装
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

private:
	// 所有リソース
	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	ModelCommon* modelCommon_ = nullptr;

	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	ParticleManager* particleManager_ = nullptr;
	RailCamera* railCamera_ = nullptr;
	Object3d* skyDome_ = nullptr;

	// シングルトン参照（プロジェクト側 GetInstance() がポインタを返す想定）
	Input* input_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;
	ObJect3dCommon* object3dCommon_ = nullptr;

	// スプライト群（所有）
	Sprite* title_ = nullptr;
	Sprite* titleUI_ = nullptr;
	Sprite* backGround_ = nullptr;
	Sprite* fadeSprite_ = nullptr;
	Sprite* Ready_ = nullptr;
	Sprite* Go_ = nullptr;
	Sprite* Black_ = nullptr;
	Sprite* gameOver_ = nullptr;
	Sprite* clear_ = nullptr;

	// UI / フェード等
	bool isTitle_ = true;
	bool isFade_ = false;
	bool endFade_ = false;
	bool isStart_ = false;
	bool isGo_ = false;
	bool isGame_ = false;
	bool isOver_ = false;
	bool isClear_ = false;
	bool reup_ = false;

	// フェード用
	Fade* fadeEffect_ = nullptr;

	// スタートタイマー等
	int titleTime_ = 0;
	int startTime_ = 0;
	int goTime_ = 0;

	// 一時 / デバッグ
	bool mouseLeft_ = false;
	bool mouseRight_ = false;

	// UV transform 用（main.cpp の変数を保持）
	Sprite::Transform uvTransformSprite_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	// helper
	void LoadCommonResources();
	void CreateScene();
	void ReleaseResources();
};