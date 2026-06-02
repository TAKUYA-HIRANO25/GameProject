#include "Bullet.h"
#include <windows.h>
#include <cstdio>

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
