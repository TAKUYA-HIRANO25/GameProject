#include "EnemyBullet.h"

// EnemyBullet:
// - 敵が発射する弾の実装（PlayerBullet とほぼ対称）。
// - Initialize でモデルを生成し、Update で移動と寿命管理、描画用行列更新を行う。
// - OnCollision で即座に無効化され、所有リスト側で解放される想定。

EnemyBullet::EnemyBullet() {

}

EnemyBullet::~EnemyBullet() {
    // デストラクタ: 所有する Object3d を解放
    delete Model_;
}

// Initialize:
// - object3dCommon を保持し、Bullet.obj をセット、初期位置と速度を保存する。
void EnemyBullet::Initialize(ObJect3dCommon* object3dCommon, const Vector3& position, const Vector3& velocity) {
    object3dCommon_ = object3dCommon;
    Model_ = new Object3d();
    Model_->Initialize(object3dCommon);
    Model_->SetModel("Bullet.obj");
    Model_->SetTranslate(position);
    velocity_ = velocity;
    position_ = position;
}

// Update:
// - 位置に速度を加算し、寿命タイマーを減らす。
// - Model_->SetTranslate / Updata を呼んで描画用行列を更新。
void EnemyBullet::Update() {
    position_ += velocity_;

    if (--deathTimer_ <= 0) {
        isDead_ = true;
    }

    // モデルに位置を反映して行列更新
    Model_->SetTranslate(position_);
    Model_->Updata();
}

// Draw:
// - モデルを描画する（描画状態は呼び出し元で確保）。
void EnemyBullet::Draw() {
    Model_->Draw();
}

// OnCollision:
// - 衝突時に弾を無効化（所有側で削除される）。
void EnemyBullet::OnCollision()
{
    isDead_ = true;
}

// GetWorldPosition:
// - 現在のワールド位置を返すユーティリティ。
Vector3 EnemyBullet::GetWorldPosition()
{
    Vector3 worldPos;
    worldPos = position_;

    return worldPos;
}