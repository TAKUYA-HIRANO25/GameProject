#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(){

}

PlayerBullet::~PlayerBullet(){
	delete Model_;
}

void PlayerBullet::Initialize(ObJect3dCommon* object3dCommon,  const Vector3& position, const Vector3& velocity) {
    object3dCommon_ = object3dCommon;  
    Model_ = new Object3d();  
    Model_->Initialize(object3dCommon);
    Model_->SetModel("Bullet.obj");
	Model_->SetTranslate(position);
    velocity_ = velocity;
	position_ = position;
}

void PlayerBullet::Update(){

	position_ += velocity_;

    if (--deathTimer_ <= 0) {
        isDead_ = true;
    }

    Model_->SetTranslate(position_);

    Model_->Updata();
}

void PlayerBullet::Draw() {
    Model_->Draw();
}

