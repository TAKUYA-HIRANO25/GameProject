#include "EnemyBullet.h"

EnemyBullet::EnemyBullet() {

}

EnemyBullet::~EnemyBullet() {
    // デストラクタ:所有するObject3dを解放
    delete Model_;
}

void EnemyBullet::Initialize(ObJect3dCommon* object3dCommon, const Vector3& position, const Vector3& velocity) {
    object3dCommon_ = object3dCommon;
    Model_ = new Object3d();
    Model_->Initialize(object3dCommon);
    Model_->SetModel("Bullet.obj");
    Model_->SetTranslate(position);
    velocity_ = velocity;
    position_ = position;
}

void EnemyBullet::Update() {
    position_ += velocity_;

    if (--deathTimer_ <= 0) {
        isDead_ = true;
    }

    // モデルに位置を反映して行列更新
    Model_->SetTranslate(position_);
    Model_->Updata();
}

void EnemyBullet::Draw() {
    Model_->Draw();
}

void EnemyBullet::OnCollision()
{
    isDead_ = true;
}

Vector3 EnemyBullet::GetWorldPosition()
{
    Vector3 worldPos;
    worldPos = position_;

    return worldPos;
}