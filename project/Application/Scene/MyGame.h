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
#include "Poose.h" 
#include "TitleScene.h"
#include "GameScene.h"

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

	//シーン
	TitleScene* titleScene_ = nullptr;
	GameScene* gameScene_ = nullptr;

	//オブジェクト
	RailCamera* railCamera_ = nullptr;

	// シングルトン参照（プロジェクト側 GetInstance() がポインタを返す想定）
	Input* input_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;
	ObJect3dCommon* object3dCommon_ = nullptr;

	// UI / フェード等
	bool isFade_ = false;
	bool endFade_ = false;
	bool isStart_ = false;
	bool isGo_ = false;
	bool isGame_ = false;
	bool isOver_ = false;
	bool isClear_ = false;
	bool reup_ = false;
	bool isScene_ = false;
	// フェード用
	Fade* fadeEffect_ = nullptr;

	// フェード完了後のプリスタート演出待機フラグ
	bool waitingPreStartCinematic_ = false;

	// UV transform 用（main.cpp の変数を保持）
	Sprite::Transform uvTransformSprite_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	enum class Scene {
		Title,
		Rule,
		Poose,
		Select,
		Game,
	};
	Scene currentScene = Scene::Title; // 初期シーンはTitle

	// helper
	void CreateScene();
	void ReleaseResources();
};