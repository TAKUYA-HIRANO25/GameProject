#pragma once
#include "Framework.h"
#include <vector>
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
#include "RailCamera.h"
#include "Fade.h"

/// <summary>
/// TitleScene
///
/// 概要:
/// - タイトル画面を表すシーンクラス。
/// - タイトル UI の表示・入力受付(ゲーム開始選択)・背景および音声の管理を行う。
/// 
/// 主な機能:
/// - Initialize:リソースや参照する共通オブジェクトを設定する。
/// - Update:入力チェックやUIの更新を行い、ゲーム開始の選択状態を管理する。
/// - Draw:タイトル画面の描画を行う。
/// - Finalize:所有リソースの解放や終了処理を行う。
/// - IsGameStartSelected():ユーザがゲーム開始を選択したかを管理する。
/// 
/// 注意:
/// - 各種共通オブジェクト(ObJect3dCommon/SpriteCommon/Audio等)は外部で作成され、ここでは参照として扱われる想定。
/// </summary>

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
	Audio* audio_ = nullptr;
	// シングルトン参照
	Input* input_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;
	ObJect3dCommon* object3dCommon_ = nullptr;

	// スプライト群
	Sprite* title_ = nullptr;
	Sprite* titleUI_ = nullptr;
	Sprite* backGround_ = nullptr;

	// UI/フェード等
	bool isTitle_ = true;

	// ゲーム開始選択状態
	bool goToGame = false;
};

