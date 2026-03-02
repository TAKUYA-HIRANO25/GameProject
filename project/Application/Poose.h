#pragma once
#include "Input.h"
#include "WinApp.h"
#include "Sprite.h"
#include "SpriteCommon.h"

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

