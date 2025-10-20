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
#include "PlayerBullet.h"

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

	//移動
	void Move();

	//弾発射
	void Fire();

	Vector3 position = { 0.0f, 1.0f, -10.0f }; // 位置

	bool bulletActive = false; //弾発射フラグ
private:
	// 基盤
	ObJect3dCommon* object3dCommon_ = nullptr;
	//プレイヤー
	Transform modelTransform_;
	Object3d* Model_ = nullptr; // 3Dオブジェクト
	Vector3 rotation; // 回転
	Vector3 scale; // 拡大縮小
	float speed; // 移動速度
	// 入力関連
	Input* input_;
	//弾関連
	PlayerBullet* Bullet_ = nullptr;
	std::list<PlayerBullet*> bulletList_;
	int bulletTime = 0;
	int bulletFlag = 0;


};

