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
/// PlayerBullet
///
/// 概要:
/// - プレイヤーが発射する弾を表す軽量クラス。位置・速度・回転・スケールを保持し、内部でObject3dを用いて描画する。
/// - 寿命管理（フレーム単位）と当たり判定による消滅処理を持つエンティティ。
///
/// 主な機能:
/// - Initialize:
///     初期位置・初期速度を設定し、描画用のObject3dを準備する。
/// - Update:
///     位置の更新・寿命のデクリメント・当たり判定や状態遷移を行う。
/// - Draw():
///     内部 `Object3d` を用いて描画を行う。
/// - OnCollision:
///     衝突時の処理を行う。
/// - IsDead/GetWorldPosition:
///     生死判定と現在位置取得用のユーティリティ。
///
/// 注意:
/// - ObJect3dCommonやObject3dの所有権・ライフサイクルは呼び出し側と整合させてください。
/// - 現在の寿命管理はフレーム依存です。必要に応じて時間ベースに変更してください。
/// - スレッドセーフは保証していないため、マルチスレッド環境では外部で同期を行ってください。
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

