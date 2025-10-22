#pragma once
#include "Object3d.h"
#include "MyMath.h"
#include "Object3dCommon.h"
#include "Input.h"
#include "WinApp.h"
#include "ModelManager.h"
#include "Model.h"
#include "ModelCommon.h"
#include "Camera.h"
#include "MatuilityForText.h"

class Enemy
{
public:
	Enemy();
	~Enemy();
	void Initialize(ObJect3dCommon* object3dCommon, Vector3 position);
	void Update();
	void Draw();

private:
	// 基盤
	ObJect3dCommon* object3dCommon_ = nullptr;
	//プレイヤー
	Transform modelTransform_;
	Object3d* Model_ = nullptr; // 3Dオブジェクト
	Vector3 position_; // 位置
	Vector3 rotation_; // 回転
	Vector3 scale_; // 拡大縮小
	float speed; // 移動速度
};

