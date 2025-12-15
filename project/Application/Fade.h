#pragma once
#include "Sprite.h"
#include "SpriteCommon.h"
#include "Vector2.h"
#include "Vector4.h"
#include "MyMath.h"

class Fade {
public:
	enum class State {
		Idle,
		Expanding,
		Shrinking,
		Finished
	};

	Fade(SpriteCommon* spriteCommon, const std::string& texturePath);
	~Fade();

	// 開始（エクスパンド→自動でシリンクは呼ばれない）
	void Start();

	// 強制的に縮小フェーズに移行
	void StartShrink();

	// 毎フレーム更新
	void Update();

	// 描画
	void Draw();

	// リセットして待機状態へ
	void Reset();

	// 状態問い合わせ
	State GetState() const { return state_; }
	bool IsRunning() const { return state_ == State::Expanding || state_ == State::Shrinking; }
	bool IsFinished() const { return state_ == State::Finished; }
	bool IsExpanding() const { return state_ == State::Expanding; }
	bool IsShrinking() const { return state_ == State::Shrinking; }

	// 現在のサイズ / 位置取得
	Vector2 GetSize() const { return size_; }
	Vector2 GetPosition() const { return position_; }

	// 設定
	void SetCenterPosition(const Vector2& center) { position_ = center; if (sprite_) sprite_->SetPosition(center); }
	void SetSpeed(float sx, float sy) { speedX_ = sx; speedY_ = sy; }
	void SetMaxSize(float w, float h) { maxWidth_ = w; maxHeight_ = h; }

	// 追加: フェード色を動的に変更する setter
	void SetColor(const Vector4& color);

private:
	SpriteCommon* spriteCommon_ = nullptr;
	Sprite* sprite_ = nullptr;
	std::string texturePath_;

	State state_ = State::Idle;

	Vector2 size_{ 0,0 };
	Vector2 position_{ 630.0f, 360.0f };

	float speedX_ = 16.0f;
	float speedY_ = 10.0f;
	float maxWidth_ = 1800.0f;
	float maxHeight_ = 1000.0f; // 任意

	// フェード色（デフォルト: 黒）
	Vector4 color_{ 0.0f, 0.0f, 0.0f, 1.0f };
};