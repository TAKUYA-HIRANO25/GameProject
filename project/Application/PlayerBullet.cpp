#include "PlayerBullet.h"

// PlayerBullet:
// - プレイヤーが発射する弾の実装。
// - 所有する Object3d(Model_) に位置を反映し、寿命（deathTimer_）で自殺する。
// - Initialize で初期位置・速度を設定し、Update で移動と寿命管理、描画用の行列更新を行う。

PlayerBullet::PlayerBullet() {
    // コンストラクタ: メンバ変数は Initialize() で初期化される想定
}

PlayerBullet::~PlayerBullet() {
    // デストラクタ: 所有する Object3d を解放
    delete Model_;
}

// Initialize:
// - object3dCommon を保持し、Bullet.obj をロードして初期位置を設定する。
// - velocity は Update で位置に加算される（ワールド空間の速度）。
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

// Update:
// - 位置に速度を加算し、寿命タイマーを減らす。
// - Model_->SetTranslate / Updata を呼んで描画用行列を更新する。
// - isDead_ は寿命切れや OnCollision() によって true になる。
void PlayerBullet::Update() {

    position_ += velocity_;

    if (--deathTimer_ <= 0) {
        isDead_ = true;
    }

    // モデルに位置を反映して行列更新
    Model_->SetTranslate(position_);
    Model_->Updata();
}

// Draw:
// - モデルの Draw を呼び出す（描画状態は呼び出し側で PSO 等を整える想定）。
void PlayerBullet::Draw() {
    Model_->Draw();
}

// OnCollision:
// - 衝突時に弾を無効化する。所有側で IsDead() を確認して解放される。
void PlayerBullet::OnCollision()
{
    isDead_ = true;
}

// GetWorldPosition:
// - 現在のワールド位置を返すユーティリティ。
Vector3 PlayerBullet::GetWorldPosition()
{
    Vector3 worldPos;
    worldPos = position_;

    return worldPos;
}