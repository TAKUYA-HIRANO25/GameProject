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
#include "EnemyBullet.h"

class Player;

class Enemy
{
public:
	Enemy();
	
	~Enemy();
	
	void Initialize(ObJect3dCommon* object3dCommon, Vector3 position);
	
	void Update();
	
	void Draw();

	void Fire();

	void FireTime();

	void MoveTime();

	Vector3 GetWorldPosition();

	void OnCollision();

	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }

	bool IsDead() const { return isDead_; }

	void setPlayer(Player* player) { player_ = player; }

	bool bulletActive = false; //弾発射フラグ

	static const int kFireInterval = 90; //弾の間隔

	static const int kMoveInterval = 360; //移動切り替え
private:
	// 基盤
	ObJect3dCommon* object3dCommon_ = nullptr;
	//敵の3Dモデル
	Transform modelTransform_;
	Object3d* Model_ = nullptr; // 3Dオブジェクト
	Vector3 position_; // 位置
	Vector3 rotation_; // 回転
	Vector3 scale_; // 拡大縮小
	float speed; // 移動速度
	//弾
	std::list<EnemyBullet*> bullets_;
	int Time = 0; //弾発射間隔用タイマー
	// 敵のHP
	float EnemyHp = 10.0f;
	//死亡フラグ
	bool isDead_ = false;
	//プレイヤー情報
	Player* player_ = nullptr;
	//移動
	Vector3 move = { 0.1f,0.0f,0.0f };
	int32_t moveTime = 0;
};

