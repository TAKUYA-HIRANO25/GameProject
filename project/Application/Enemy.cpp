#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
	delete Model_;
}

void Enemy::Initialize(ObJect3dCommon* object3dCommon, Vector3 position)
{
	object3dCommon_ = object3dCommon;
	position_ = position;
	Model_ = new Object3d();
	Model_->Initialize(object3dCommon);
	Model_->SetModel("box.obj");
	Model_->SetTranslate(position);
}

void Enemy::Update()
{
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	Fire();

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	Model_->SetTranslate(position_);

	Model_->Updata();
}

void Enemy::Draw()
{
	Model_->Draw();
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw();
	}
}


void Enemy::Fire() {
	bulletTime++;

	if (bulletTime >= 30) {
		bulletTime = 0;
		bulletFlag = true;
	}

	if(bulletFlag) {
		bulletActive = true;
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		bulletFlag = false;

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(object3dCommon_, Model_->GetTranslate(), velocity);
		bullets_.push_back(newBullet);
	}


}