#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
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
	Model_->SetTranslate(position_);

	Model_->Updata();
}

void Enemy::Draw()
{
	Model_->Draw();
}
