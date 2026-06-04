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
	Model_->SetModel("Bullet/Bullet.obj");
	Model_->SetTranslate(position_);
	Model_->SetScale({ 0.8f, 0.8f, 0.8f });
	Model_->SetDiffuseColor({ 1.0f, 0.85f, 0.0f, 1.0f });
	// 初期化直後に行列更新
	Model_->Updata();

	// 初期化
	Bullet::Initialize(position.x, position.y, position.z, velocity.x, velocity.y, velocity.z,kLifeTime);
}

void PlayerBullet::Update()
{
	// 基底で位置更新、寿命管理
	Bullet::Update();

	// 描画用のVector3に反映
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