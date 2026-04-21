#include "PlayerBullet.h"

PlayerBullet::PlayerBullet() {
    // Initializerで初期化
}

PlayerBullet::~PlayerBullet() {
    // デストラクタ: 所有するObject3dを解放
    delete Model_;
}

void PlayerBullet::Initialize(ObJect3dCommon* object3dCommon, const Vector3& position, const Vector3& velocity) {
    object3dCommon_ = object3dCommon;
    Model_ = new Object3d();
    Model_->Initialize(object3dCommon);
    Model_->SetModel("Bullet.obj");
    Model_->SetTranslate(position);
    velocity_.x = velocity.x * speed_;
	velocity_.y = velocity.y * speed_;
	velocity_.z = velocity.z * speed_;
    position_ = position;
}

void PlayerBullet::Update() {

    position_ += velocity_;

    if (--deathTimer_ <= 0) {
        isDead_ = true;
    }

    // モデルに位置を反映して行列更新
    Model_->SetTranslate(position_);
    Model_->Updata();
}

void PlayerBullet::Draw() {
    Model_->Draw();
}

void PlayerBullet::OnCollision()
{
    isDead_ = true;
}

Vector3 PlayerBullet::GetWorldPosition()
{
    Vector3 worldPos;
    worldPos = position_;

    return worldPos;
}