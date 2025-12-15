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
	if (!sprite_) return;
	// Idle または Finished から開始する
	if (state_ == State::Idle || state_ == State::Finished) {
		state_ = State::Expanding;
		size_ = { 0.0f, 0.0f };

		// sprite に値をセットし、必ず GPU バッファに反映する
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
	// それ以外は強制的に縮小へ移行
	state_ = State::Shrinking;
	// 即座に反映（必要なら）
	if (sprite_) {
		sprite_->SetColor(color_);
		sprite_->Update();
	}
}

void Fade::Update()
{
	if (!sprite_) return;

	if (state_ == State::Expanding) {
		size_.x += speedX_;
		size_.y += speedY_;
		position_.x -= speedX_ / 2.0f;
		position_.y -= speedY_ / 2.0f;
		// 値をまとめて sprite に反映してから Update()
		sprite_->SetSize(size_);
		sprite_->SetPosition(position_);
		sprite_->SetColor(color_);
		sprite_->Update();

		if (size_.x >= maxWidth_) {
			// 到達時は縮小フェーズに移行
			state_ = State::Shrinking;
		}
	}
	else if (state_ == State::Shrinking) {
		size_.x -= speedX_;
		size_.y -= speedY_;
		position_.x += speedX_ / 2.0f;
		position_.y += speedY_ / 2.0f;
		if (size_.x < 0.0f) size_.x = 0.0f;
		if (size_.y < 0.0f) size_.y = 0.0f;

		sprite_->SetSize(size_);
		sprite_->SetPosition(position_);
		sprite_->SetColor(color_);
		sprite_->Update();

		if (size_.x <= 0.0f && size_.y <= 0.0f) {
			state_ = State::Finished;
		}
	}
	// Idle / Finished は基本何もしない
}

void Fade::Draw()
{
	if (!sprite_) return;
	if (state_ == State::Idle) return;

	// 描画直前に最新のバッファへ反映（Update が呼ばれていないケースに備える）
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

// 追加: フェード色を動的に変更する実装
void Fade::SetColor(const Vector4& color)
{
	color_ = color;
	if (sprite_) {
		sprite_->SetColor(color_);
		// 色変更は即時反映して問題にならないため Update を呼ぶ
		sprite_->Update();
	}
}

void Fade::SetCenterPosition(const Vector2& center)
{
	position_ = center;
	if (sprite_) {
		sprite_->SetPosition(center);
		// 位置変更は即時反映
		sprite_->Update();
	}
}