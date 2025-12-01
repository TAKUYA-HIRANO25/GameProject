#pragma once
#include "MatuilityForText.h"
#include "Camera.h"

class RailCamera
{
public:
	void Initialize(Vector3& translate, Vector3& rotate);

	void Update();

	Camera& GetCamera() { return camera_; }

	Transform& GetTransform() { return  transform_; }

private:
	//カメラ位置
	Transform transform_; // 拡縮,回転,移動
	//カメラ速度
	Vector3 velocity_;
	//カメラ
	Camera camera_;
};

