#include "Fade.h"
#include <cassert>

Fade::Fade(SpriteCommon* spriteCommon, const std::string& texturePath)
	: spriteCommon_(spriteCommon)
	, texturePath_(texturePath)
{
	assert(spriteCommon_);
	// Sprite を作成して初期化。テクスチャは先にロード済みが前提。
	sprite_ = new Sprite();
	sprite_->Initialize(spriteCommon_, texturePath_);
	sprite_->SetSize(Vector2(0.0f, 0.0f));
	sprite_->SetPosition(position_);
	// 初期色を適用
	sprite_->SetColor(color_);
	state_ = State::Idle;
}

Fade::~Fade()
{
	delete sprite_;
	sprite_ = nullptr;
}

void Fade::Start()
{
	if (state_ == State::Idle || state_ == State::Finished) {
		state_ = State::Expanding;
		size_ = { 0.0f, 0.0f };
		sprite_->SetSize(size_);
		sprite_->SetPosition(position_);
		// 色も確実に適用
		sprite_->SetColor(color_);
	}
}

void Fade::StartShrink()
{
	// 既に縮小中なら無視
	if (state_ == State::Shrinking) {
		return;
	}
	// それ以外は強制的に縮小フェーズへ
	state_ = State::Shrinking;
}

void Fade::Update()
{
	if (!sprite_)
	{
		return;
	}

	if (state_ == State::Expanding) {
		position_.x -= speedX_ / 2.0f;
		position_.y -= speedY_ / 2.0f;
		size_.x += speedX_;
		size_.y += speedY_;
		sprite_->SetSize(Vector2(size_.x, size_.y));
		sprite_->SetPosition(position_);
		sprite_->SetColor(color_);
		// GPUバッファに反映
		sprite_->Update();
		if (size_.x >= maxWidth_) {
			state_ = State::Shrinking;
		}
	}
	else if (state_ == State::Shrinking) {
		position_.x += speedX_ / 2.0f;
		position_.y += speedY_ / 2.0f;
		size_.x -= speedX_;
		size_.y -= speedY_;
		if (size_.x < 0.0f) size_.x = 0.0f;
		if (size_.y < 0.0f) size_.y = 0.0f;
		sprite_->SetSize(Vector2(size_.x, size_.y));
		sprite_->SetPosition(position_);
		sprite_->SetColor(color_);
		// GPUバッファに反映
		sprite_->Update();
		if (size_.x <= 0.0f && size_.y <= 0.0f) {
			state_ = State::Finished;
		}
	}
	// Idle / Finished は何もしない（必要ならここで sprite_->Update() しておく）
}

void Fade::Draw()
{
	if (!sprite_) return;
	if (state_ == State::Idle) return;
	// 念のため直前にバッファ反映（Update が呼ばれなかったケースに備える）
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
	}
}

// 追加: フェード色を動的に変更する実装
void Fade::SetColor(const Vector4& color)
{
	color_ = color;
	if (sprite_) {
		sprite_->SetColor(color_);
	}
}