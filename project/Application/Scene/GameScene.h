#pragma once
#include "Framework.h"
#include <vector>
#include <memory>
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Audio.h"
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
#include "FramWork/GameObject.h" 
#include "Bullet.h"
#include "GameSceneState.h" // 追加

/// <summary>
/// GameScene
///
/// 概要:
/// - メインゲームプレイを管理するシーンクラス。
/// - プレイヤー、敵、パーティクル、UIや時間管理、シーン遷移を制御。
/// 
/// 主な機能:
/// - Initialize:シーン内で使用するオブジェクトや参照を初期化。
/// - Update:ゲームロジック(入力、物理、敵挙動、エフェクト、シーン状態遷移)を毎フレーム実行。
/// - Draw:3D/2Dの描画(オブジェクト、スプライト)を行う。
/// - Finalize:所有リソースの解放や終了処理を行う。
/// - IsGameSet:シーンのセットアップ完了状態を管理。
/// 
/// 注意:
/// - シーン内のオブジェクト所有権とライフサイクルは本クラスの実装に依存するため、外部呼び出し側と整合させること。
/// </summary>

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

	// Stateパターン用API
	void RequestStateChange(std::unique_ptr<GameSceneState> newState) { pendingState_ = std::move(newState); }

	// Scene 별処理を State から呼べるように公開メソッドに分離
	void UpdateReady();
	void UpdateGo();
	void UpdateMain();
	void UpdateGameOver();
	void UpdateClear();

	void DrawReady();
	void DrawGo();
	void DrawMain();
	void DrawGameOver();
	void DrawClear();

private:
	// 所有リソース
	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	RailCamera* railCamera_ = nullptr;
	ModelCommon* modelCommon_ = nullptr;
	Audio* audio_ = nullptr;
	//オブジェクト
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	// GameObjectベースで一括管理
	std::vector<GameObject*> objects_;

	Object3d* skyDome_ = nullptr;
	ParticleManager* particleManager_ = nullptr;
	//スプライト
	Sprite* Ready_ = nullptr;
	Sprite* Go_ = nullptr;
	Sprite* Black_ = nullptr;
	Sprite* gameOver_ = nullptr;
	Sprite* clear_ = nullptr;
	Sprite* EndUI_ = nullptr;
	Sprite* explanation_ = nullptr;

	// Pooseの追加
	Poose* poose_ = nullptr;

	// フラグ
	bool isGame_ = false;
	bool isSet_ = false;
	// スタートタイマー等
	int startTime_ = 0;
	int goTime_ = 0;

	// シングルトン参照
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

	// Stateパターン用メンバ
	std::unique_ptr<GameSceneState> state_;
	std::unique_ptr<GameSceneState> pendingState_;
};

