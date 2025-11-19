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
/// <summary>
/// プレイヤーのを動かすクラス
/// </summary>
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

	bool bulletActive = false; //弾発射フラグ

	Vector3 GetWorldPosition();

	void OnCollision();

	const std::list<PlayerBullet*>& GetBullets() const { return bulletList_; }

	void Reticle();

	bool IsDead() const { return isDead_; }
private:
	// 基盤
	ObJect3dCommon* object3dCommon_ = nullptr;
	//プレイヤー
	Transform modelTransform_;
	Object3d* Model_ = nullptr; // 3Dオブジェクト
	Vector3 position_ = { 0.0f, 1.0f, 10.0f }; // 位置
	Vector3 rotation; // 回転
	Vector3 scale; // 拡大縮小
	float speed; // 移動速度
	// 入力関連
	Input* input_;
	//弾関連
	std::list<PlayerBullet*> bulletList_;
	int bulletTime = 0; //弾発射間隔用タイマー
	int bulletFlag = 0;

	// --- レティクル関連 ---
	Object3d* reticleModel_ = nullptr;    // 画面上の3Dレティクル表示用
	Vector3 reticleWorldPos_ = { 0.0f, 0.0f, 0.0f }; // レティクルのワールド位置
	float reticleDistance_ = 100.0f; // レティクルまでの距離（代替ターゲット距離）

	float PlayerHP = 5.0f;
	bool isDead_ = false;
};