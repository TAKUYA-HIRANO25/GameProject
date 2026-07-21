#pragma once
#include "Input.h"
#include "WinApp.h"
#include "Sprite.h"
#include "SpriteCommon.h"

/// <summary>
/// Poose
/// 
/// 概要:
/// - ゲームプレイ中のポーズメニューを管理するクラス。
/// 
/// 主な機能:
/// - Initialize:SpriteCommonとの参照を受け取って初期化。
/// - Activate/Deactivate:ポーズの有効化・無効化を切り替える。Activateで入力ロックも行う。
/// - IsActive:ポーズが現在有効かを問い合わせる。
/// - Update:ポーズメニューの入力処理と状態更新を行う。選択肢の切り替えや決定を処理し、結果をセット。
/// - Draw:ポーズメニューのスプライトを描画。
/// - GetResult/ClearResult:ポーズメニューで選択された結果を取得・クリア。
/// 
/// 注意:
/// - PooseがActivateされている間は、Inputのキーボードとマウス入力がロックされるため、ゲームプレイ側はこれを考慮して入力処理を行う必要がある。 
/// </summary>

class Poose
{
public:
	enum class Result {
		None,
		Resume,
		ToTitle,
	};

	Poose();
	~Poose();

	// 初期化（SpriteCommon と Input を受け取る）
	void Initialize();

	// ポーズの有効化 / 無効化
	void Activate() noexcept;
	void Deactivate() noexcept;
	bool IsActive() const noexcept;

	// 更新・描画
	void Update();
	void Draw();

	// 結果取得 / クリア
	Result GetResult() const noexcept;
	void ClearResult() noexcept;

private:
	Input* input_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;

	// 表示用スプライト
	Sprite* overlaySprite_ = nullptr;
	Sprite* resumeSprite_ = nullptr;

	// 状態
	bool active_ = false;
	int selectedIndex_ = 0; // 0 = Resume, 1 = Title
	Result result_ = Result::None;

	// 内部ヘルパー
	void EnsureSpritesInitialized();
	void UpdateInput();
	void UpdateSprites();
};

