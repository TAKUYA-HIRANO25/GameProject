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
#include "RailCamera.h"
#include "Fade.h"

class TitleScene {
public:
	TitleScene();
	~TitleScene();
	// 初期化
	void Initialize(RailCamera* railCamera);
	// 更新
	void Update();
	// 描画
	void Draw();
	// ゲーム開始選択状態の取得
	bool IsGameStartSelected() const { return goToGame; }
	// 終了
	void Finalize();
private:
	// 所有リソース
	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	RailCamera* railCamera_ = nullptr;
	ModelCommon* modelCommon_ = nullptr;

	// シングルトン参照（プロジェクト側 GetInstance() がポインタを返す想定）
	Input* input_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;
	ObJect3dCommon* object3dCommon_ = nullptr;

	// スプライト群（所有）
	Sprite* title_ = nullptr;
	Sprite* titleUI_ = nullptr;
	Sprite* backGround_ = nullptr;

	// UI / フェード等
	bool isTitle_ = true;

	// ゲーム開始選択状態
	bool goToGame = false;
};

