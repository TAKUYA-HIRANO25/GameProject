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

/// <summary>
/// プレイヤーの弾を生成するクラス
/// </summary>

class PlayerBullet
{
public:
	// コンストラクタ
	PlayerBullet();
	// デストラクタ
	~PlayerBullet();

	// 初期化
	void Initialize(ObJect3dCommon* object3dCommon, const Vector3& position, const Vector3& velocity);

	// 更新
	void Update();

	// 描画
	void Draw();

	// プレイヤーの生死判定
	bool IsDead() const { return isDead_; }

	// 当たり判定
	void OnCollision();

	// ワールド位置取得
	Vector3 GetWorldPosition();
private:
	// 基盤
	ObJect3dCommon* object3dCommon_ = nullptr;
	// 3Dモデル関連
	Transform modelTransform_;
	Object3d* Model_ = nullptr; // 3Dオブジェクト
	// 位置関連
	Vector3 position_; // 位置
	Vector3 rotation_; // 回転
	Vector3 scale_; // 拡大縮小
	Vector3 velocity_; // 移動方向
	float speed_ = 2.0f; // 移動速度
	//タイマー
	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTimer_ = kLifeTime;
	bool isDead_ = false;
};

