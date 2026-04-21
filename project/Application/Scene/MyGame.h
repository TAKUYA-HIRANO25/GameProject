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

/// <summary>
/// MyGame
///
/// 概要:
/// - Frameworkを継承したアプリケーション本体クラス。ゲーム全体の初期化・更新・描画・終了処理を実装する。
/// - シーン管理(タイトル / ゲーム 等)、主要リソース、およびカメラやフェード等の UI 制御を保持する。
/// 
/// 主な機能:
/// - Initialize:全シーンと共通リソースの生成・初期化を行う。
/// - Update:現在シーンの更新・入力処理・シーン遷移の管理を行う。
/// - Draw:現在シーンおよび UI の描画を行う。
/// - Finalize:所有する全リソース・シーンの解放を行う。
/// 
/// 注意:
/// - シーン切替やフェード完了待ちなどのフロー制御を内部で行うため、派生処理を追加する際はCreateScene/ReleaseResourcesを適切に更新すること。
/// </summary>

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