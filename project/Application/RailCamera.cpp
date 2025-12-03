#include "RailCamera.h"
RailCamera::RailCamera()
	: transform_({ {1.0f,1.0f,1.0f}, { 0.0f, 6.28f,0.0f }, { 0.0f,0.0f,-20.0f } })
	, velocity_{ 0.0f, 0.0f, 0.1f }
	, camera_(nullptr)
{
}
void RailCamera::Initialize(Camera* camera)
{
	camera_ = camera;
	if (camera_) {
		transform_ = camera_->GetTransform();

	}

}

void RailCamera::Update()
{
	//カメラ位置更新
	transform_.translate.x += velocity_.x;
	transform_.translate.y += velocity_.y;
	transform_.translate.z += velocity_.z;

	// カメラへ反映
	if (camera_) {
		camera_->SetTransform(transform_);
		camera_->Update();
	}
}
