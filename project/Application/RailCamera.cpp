#include "RailCamera.h"

// RailCamera:
// - レールに沿って移動するカメラを扱うユーティリティクラス。
// - 内部で Transform を保持し、Update 時に velocity_ を適用して Camera に反映する。
// - Camera クラス側でワールド/ビュー/射影行列の更新が行われる想定。

RailCamera::RailCamera()
	: transform_({ {1.0f,1.0f,1.0f}, { 0.0f, 6.28f,0.0f }, { 0.0f,0.0f,-20.0f } })
	, velocity_{ 0.0f, 0.0f, 0.0f }
	, camera_(nullptr)
{
	// コンストラクタ: transform_ を初期配置し、速度をゼロにしておく
}

void RailCamera::Initialize(Camera* camera)
{
	// Camera ポインタを保持し、存在すれば Camera の Transform をコピーして初期同期
	camera_ = camera;
	if (camera_) {
		// Camera 側の Transform を取得して初期化（外部で設定されたカメラ状態を尊重）
		transform_ = camera_->GetTransform();
	}
}

void RailCamera::Update()
{
	// レールに沿った移動量を Transform に加算
	transform_.translate.x += velocity_.x;
	transform_.translate.y += velocity_.y;
	transform_.translate.z += velocity_.z;

	// Camera が有効な場合は Transform をカメラへ反映し、カメラ側の Update を呼ぶ
	// Camera::Update() はビュー行列や射影行列の最新化を行う想定
	if (camera_) {
		camera_->SetTransform(transform_);
		camera_->Update();
	}
}
