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
class GameScene
{
public:
	GameScene();
	~GameScene();
	// 初期化
	void Initialize(RailCamera* railCamera);
	// 更新
	void Update();
	// 描画
	void Draw();
	// ゲーム開始選択状態の取得
	bool IsGameSet() const { return isSet_; }
	// 終了
	void Finalize();
private:
	// 所有リソース
	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	RailCamera* railCamera_ = nullptr;
	ModelCommon* modelCommon_ = nullptr;
	//オブジェクト
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	Object3d* skyDome_ = nullptr;
	ParticleManager* particleManager_ = nullptr;
	//スプライト
	Sprite* Ready_ = nullptr;
	Sprite* Go_ = nullptr;
	Sprite* Black_ = nullptr;
	Sprite* gameOver_ = nullptr;
	Sprite* clear_ = nullptr;
	Sprite* explanation_ = nullptr;

	// Poose の追加
	Poose* poose_ = nullptr;

	// フラグ
	bool isGame_ = false;
	bool isSet_ = false;
	// スタートタイマー等
	int startTime_ = 0;
	int goTime_ = 0;

	// シングルトン参照（プロジェクト側 GetInstance() がポインタを返す想定）
	Input* input_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;
	ObJect3dCommon* object3dCommon_ = nullptr;

	// シーン管理用の列挙型
	enum class Scene {
		ready,
		Go,
		main,
		gameOver,
		clear,
	};

	// 現在のシーン
	Scene currentScene_ = Scene::ready;
};

