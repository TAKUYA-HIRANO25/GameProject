#include "PlayerBullet.h"
#include <windows.h>
#include <cstdio>

PlayerBullet::PlayerBullet() = default;
PlayerBullet::~PlayerBullet()
{
	delete Model_;
	Model_ = nullptr;
}

void PlayerBullet::Initialize(ObJect3dCommon* object3dCommon, const Vector3& position, const Vector3& velocity)
{
	object3dCommon_ = object3dCommon;
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;

	// Object3d 初期化
	Model_ = new Object3d();
	Model_->Initialize(object3dCommon_);
	// デバッグ: 確実に存在するモデルを一時使用して可視化を確認
	// 実運用では元の "Player/Bullet.obj" に戻してください
	Model_->SetModel("Bullet/Bullet.obj");
	Model_->SetTranslate(position_);
	// 少し大きめにして見えやすくする
	Model_->SetScale({ 2.5f, 2.5f, 2.5f });
	// 目立つ色に変更（デバッグ）
	Model_->SetDiffuseColor({ 1.0f, 0.85f, 0.0f, 1.0f });
	// 初期化直後に行列更新
	Model_->Updata();

	// 基底の物理情報を初期化
	Bullet::Initialize(position.x, position.y, position.z, velocity.x, velocity.y, velocity.z,kLifeTime);
}

void PlayerBullet::Update()
{
	// 基底で位置更新・寿命管理
	Bullet::Update();

	// 基底の座標を描画用の Vector3 に反映
	GetWorldPosition(position_.x, position_.y, position_.z);

	// Model を更新
	if (Model_) {
		Model_->SetTranslate(position_);
		Model_->Updata();
	}
}

void PlayerBullet::Draw()
{
	if (!IsDead() && Model_) {
		Model_->Draw();
	}
}

void PlayerBullet::OnCollision()
{
	Bullet::OnCollision();
}

void PlayerBullet::GetWorldPosition(float& x, float& y, float& z) const
{
	Bullet::GetWorldPosition(x, y, z);
}

Vector3 PlayerBullet::GetWorldPosition() const
{
	Vector3 v;
	float x, y, z;
	GetWorldPosition(x, y, z);
	v.x = x; v.y = y; v.z = z;
	return v;
}