	#include "Bullet.h"
#include <windows.h>
#include <cstdio>

Bullet::Bullet() = default;

Bullet::~Bullet()
{
	// 3D モデルを解放
	if (model3d_) {
		delete model3d_;
		model3d_ = nullptr;
	}
}

void Bullet::Initialize(float px, float py, float pz,
	                    float vx, float vy, float vz,
	                    int32_t lifeFrames)
{
	px_ = px;
	py_ = py;
	pz_ = pz;
	vx_ = vx;
	vy_ = vy;
	vz_ = vz;
	lifeTimer_ = lifeFrames;
	isDead_ = false;

	char buf[256];
	sprintf_s(buf, "Bullet::Initialize pos(%.3f,%.3f,%.3f) vel(%.3f,%.3f,%.3f) life=%d\n",
		px_, py_, pz_, vx_, vy_, vz_, lifeTimer_);
	OutputDebugStringA(buf);
}

void Bullet::Initialize3D(ObJect3dCommon* object3dCommon,
                          const Vector3& position, const Vector3& velocity,
                          const std::string& modelPath,
                          const Vector4& diffuseColor,
                          int32_t lifeFrames,
                          Type ownerType)
{
	// 基本パラメータを初期化
	Initialize(position.x, position.y, position.z, velocity.x, velocity.y, velocity.z, lifeFrames);

	object3dCommon_ = object3dCommon;
	modelPosition_ = position;
	ownerType_ = ownerType;

	// Object3d 初期化
	model3d_ = new Object3d();
	model3d_->Initialize(object3dCommon_);
	model3d_->SetModel(modelPath.c_str());
	model3d_->SetTranslate(modelPosition_);
	model3d_->SetScale({ 0.8f, 0.8f, 0.8f });
	model3d_->SetDiffuseColor(diffuseColor);
	// 初期化直後に行列更新
	model3d_->Updata();
}

void Bullet::Update()
{
	// 位置更新
	px_ += vx_;
	py_ += vy_;
	pz_ += vz_;

	// 寿命管理
	if (lifeTimer_ > 0) {
		--lifeTimer_;
		if (lifeTimer_ <= 0) {
			isDead_ = true;
		}
	}

	// 3D モデルがある場合、モデルの位置を更新して行列を更新
	if (model3d_) {
		modelPosition_.x = px_;
		modelPosition_.y = py_;
		modelPosition_.z = pz_;
		model3d_->SetTranslate(modelPosition_);
		model3d_->Updata();
	}
}

void Bullet::Draw()
{
	if (!IsDead() && model3d_) {
		model3d_->Draw();
	}
}

void Bullet::OnCollision()
{
	// デフォルトは即時消滅
	isDead_ = true;
}

void Bullet::GetWorldPosition(float& x, float& y, float& z) const
{
	x = px_;
	y = py_;
	z = pz_;
}

void Bullet::GetVelocity(float& x, float& y, float& z) const
{
	x = vx_;
	y = vy_;
	z = vz_;
}

void Bullet::SetVelocity(float vx, float vy, float vz)
{
	vx_ = vx;
	vy_ = vy;
	vz_ = vz;
}
