#include "Fade.h"
#include <cassert>

#define USE_SPRITE

Fade::Fade(SpriteCommon* spriteCommon, const std::string& texturePath)
	: spriteCommon_(spriteCommon)
	, texturePath_(texturePath)
{
	// コンストラクタ: SpriteCommon は必須
	assert(spriteCommon_);

	// Sprite を作成して初期化（テクスチャは事前ロード済みが前提）
	sprite_ = new Sprite();
	sprite_->Initialize(spriteCommon_, texturePath_);
	// 初期は表示無し（サイズ0）、中心位置は position_ の初期値を使う
	sprite_->SetSize(Vector2(0.0f, 0.0f));
	sprite_->SetPosition(position_);
	// 初期色を適用
	sprite_->SetColor(color_);
	state_ = State::Idle;
}

Fade::~Fade()
{
	// デストラクタ: Sprite を解放
	delete sprite_;
	sprite_ = nullptr;
}

void Fade::Start()
{
	if (!sprite_) return;
	if (state_ == State::Idle || state_ == State::Finished) {
		state_ = State::Expanding;
		size_ = { 0.0f, 0.0f };

		// Sprite に反映して GPU 側へ更新
		sprite_->SetSize(size_);
		sprite_->SetPosition(position_);
		sprite_->SetColor(color_);
		sprite_->Update();
	}
}

void Fade::StartShrink()
{
	// 既に縮小中なら何もしない
	if (state_ == State::Shrinking) {
		return;
	}
	// 強制的に縮小へ移行
	state_ = State::Shrinking;

	// 表示色などが変わる可能性があるため即時反映
	if (sprite_) {
		sprite_->SetColor(color_);
		sprite_->Update();
	}
}

void Fade::Update()
{
	if (!sprite_) return;

	if (state_ == State::Expanding) {
		// 拡大処理：サイズを増やし、見た目上中央に広がるよう位置を少しずらす
		size_.x += speedX_;
		size_.y += speedY_;
		position_.x -= speedX_ / 2.0f;
		position_.y -= speedY_ / 2.0f;

		// Sprite へまとめて反映して GPU 更新
		sprite_->SetSize(size_);
		sprite_->SetPosition(position_);
		sprite_->SetColor(color_);
		sprite_->Update();

		// 最大サイズに到達したら縮小フェーズへ
		if (size_.x >= maxWidth_) {
			state_ = State::Shrinking;
		}
	}
	else if (state_ == State::Shrinking) {
		// 縮小処理：サイズを減らし、中心を維持するため位置を調整
		size_.x -= speedX_;
		size_.y -= speedY_;
		position_.x += speedX_ / 2.0f;
		position_.y += speedY_ / 2.0f;

		// 下限 clamp
		if (size_.x < 0.0f) size_.x = 0.0f;
		if (size_.y < 0.0f) size_.y = 0.0f;

		// Sprite へ反映して GPU 更新
		sprite_->SetSize(size_);
		sprite_->SetPosition(position_);
		sprite_->SetColor(color_);
		sprite_->Update();

		// サイズが完全に消えたら終了状態へ
		if (size_.x <= 0.0f && size_.y <= 0.0f) {
			state_ = State::Finished;
		}
	}
	// Idle / Finished は何もしない（必要に応じて UI から Reset() を呼ぶ）
}

void Fade::Draw()
{
	if (!sprite_) return;
	if (state_ == State::Idle) return;

	// 念のため直前で Update（Update() が呼ばれていないケースや外部で直接 SetColor した場合に備える）
	sprite_->Update();
	sprite_->Draw();
}

void Fade::Reset()
{
	state_ = State::Idle;
	size_ = { 0.0f, 0.0f };
	if (sprite_) {
		sprite_->SetSize(Vector2(0.0f, 0.0f));
		sprite_->SetPosition(position_);
		sprite_->SetColor(color_);
		sprite_->Update();
	}
}

void Fade::SetColor(const Vector4& color)
{
	color_ = color;
	if (sprite_) {
		sprite_->SetColor(color_);
		sprite_->Update();
	}
}

// フェードの中心位置を変更して Sprite に反映する。
// UI レイアウト等で中心を動的に変更する用途に使う。
void Fade::SetCenterPosition(const Vector2& center)
{
	position_ = center;
	if (sprite_) {
		sprite_->SetPosition(center);
		sprite_->Update();
	}
}