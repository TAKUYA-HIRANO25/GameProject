#include "EnemyBullet.h"
#include <windows.h>
#include <cstdio>

// コンストラクタ/デストラクタ
EnemyBullet::EnemyBullet() = default;
EnemyBullet::~EnemyBullet()
{
	delete Model_;
	Model_ = nullptr;
}

// Initialize: Object3d 初期化を行い、基底の位置/速度も初期化する
void EnemyBullet::Initialize(ObJect3dCommon* object3dCommon, const Vector3& position, const Vector3& velocity)
{
	object3dCommon_ = object3dCommon;
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;

	Model_ = new Object3d();
	Model_->Initialize(object3dCommon_);
	Model_->SetModel("Bullet/Bullet.obj");
	Model_->SetTranslate(position_);
	Model_->SetScale({ 0.8f, 0.8f, 0.8f });
	Model_->SetDiffuseColor({ 1.0f, 0.3f, 0.3f, 1.0f });
	// 初期化直後に行列更新
	Model_->Updata();

	// 初期化
	Bullet::Initialize(position.x, position.y, position.z,velocity.x, velocity.y, velocity.z, kLifeTime);
}


void EnemyBullet::Update()
{
	// 基底で位置更新、寿命管理
	Bullet::Update();

	// 描画用のVector3に反映
	GetWorldPosition(position_.x, position_.y, position_.z);

	// Modelを更新
	if (Model_) {
		Model_->SetTranslate(position_);
		Model_->Updata();
	}
}

// Draw:Object3dを描画
void EnemyBullet::Draw()
{
	if (!IsDead() && Model_) {
		Model_->Draw();
	}
}

// OnCollision:衝突時の挙動
void EnemyBullet::OnCollision()
{
	Bullet::OnCollision();
}

void EnemyBullet::GetWorldPosition(float& x, float& y, float& z) const
{
	Bullet::GetWorldPosition(x, y, z);
}

Vector3 EnemyBullet::GetWorldPosition() const
{
	Vector3 v;
	float x, y, z;
	GetWorldPosition(x, y, z);
	v.x = x; v.y = y; v.z = z;
	return v;
}
