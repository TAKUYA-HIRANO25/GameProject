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
/// EnemyBullet
///
/// 概要:
/// - 敵が発射する弾を表すクラス。
/// - 位置・速度・回転・スケールを保持し、内部でObject3dを用いて描画する。
/// - 寿命管理と当たり判定による消滅処理を持つ軽量エンティティ。
///
/// 主な機能:
/// - Initialize:
///     初期位置と初期速度を指定して初期化する。内部でモデルのセットアップを行うことを想定。
/// - Update:
///     位置の更新・寿命タイマーのデクリメント・当たり判定フラグ処理など、毎フレームの更新を行う。
/// - Draw:
///     内部のObject3dを用いて弾を描画する。
/// - IsDead():
///     寿命切れや衝突で消滅済みかどうかを問い合わせる。
/// - OnCollision:
///     衝突時の処理を行う。
/// - GetWorldPosition:
///     現在のワールド空間での位置を返す。
///
/// 注意:
/// - ObJect3dCommonやObject3dのライフサイクルは外部と調整すること。所有権ルールに注意。
/// - デフォルト寿命はkLifeTimeで定義されている。必要に応じて変更して使う。
/// - 本クラスはスレッドセーフを保証しないため、マルチスレッドでアクセスする場合は同期を行ってください。
/// </summary>

class EnemyBullet
{
public:
	// コンストラクタ
	EnemyBullet();

	// デストラクタ
	~EnemyBullet();

	// 初期化
	void Initialize(ObJect3dCommon* object3dCommon, const Vector3& position, const Vector3& velocity);

	// 更新
	void Update();

	// 描画
	void Draw();

	// 生死判定
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
	Vector3 velocity_; // 移動速度
	//タイマー
	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTimer_ = kLifeTime;
	bool isDead_ = false;

};

