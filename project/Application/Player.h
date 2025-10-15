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
class Player
{
public:
	Player();
	~Player();

	// 初期化
	void Initialize(ObJect3dCommon* object3dCommon, Input* input);

	// 更新
	void Update();

	// 描画
	void Draw();

	Vector3 position = { 0.0f,1.0f,0.0f }; // 位置
private:
	// 基盤
	ObJect3dCommon* object3dCommon_ = nullptr;
	//プレイヤー
	Transform modelTransform_;
	Object3d* PlayerModel = nullptr; // 3Dオブジェクト
	Vector3 rotation; // 回転
	Vector3 scale; // 拡大縮小
	float speed; // 移動速度
	// 入力関連
	Input* input_;
};

