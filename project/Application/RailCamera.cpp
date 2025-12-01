#include "RailCamera.h"

void RailCamera::Initialize(Vector3& translate, Vector3& rotate)
{
	transform_.rotate = rotate;
	transform_.translate = translate;

	velocity_ = { 0.0f, 0.0f, 1.0f };

}

void RailCamera::Update()
{
	//カメラ位置更新
	transform_.translate.x += velocity_.x;
	transform_.translate.y += velocity_.y;
	transform_.translate.z += velocity_.z;
	//カメラ更新
	camera_.SetTransform(transform_);
	camera_.Update();
}
