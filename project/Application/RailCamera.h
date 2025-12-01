#pragma once
#include "MatuilityForText.h"
#include "Camera.h"

class RailCamera
{
public:
	//初期化
	void Initialize(Vector3& translate, Vector3& rotate);
	//更新
	void Update();
	//カメラ取得
	Camera& GetCamera() { return camera_; }
	//変換情報取得
	Transform& GetTransform() { return  transform_; }

private:
	//カメラ位置
	Transform transform_; // 拡縮,回転,移動
	//カメラ速度
	Vector3 velocity_;
	//カメラ
	Camera camera_;
};

