#pragma once
#include <memory>
#include <list>
#include "GameObject.h"
#include "EnemyBullet.h"
#include "Player.h"
#include "ParticleManager.h"
#include "Object3d.h"
#include "MyMath.h"
#include "EnemyState.h"

/// <summary>
///
/// 概要:
/// - 敵の生成、更新、描画、弾発射、被弾エフェクト等を担当するクラス実装。
/// - オブジェクトのライフサイクル管理、当たり判定時の色点滅、パーティクル生成ロジック。
///
/// 主な責務:
/// - Initialize:モデルや入力参照の初期設定。
/// - Update:移動:行動パターン・射撃処理・弾の更新・死亡判定。
/// - Draw:敵本体と弾の描画を行う。
/// - Move/Fire:行動パターンに基づく移動と弾発射処理。
///
/// 注意事項:
/// - 弾は生ポインタで管理されるため、外部参照がある場合は破棄タイミングに注意。
/// - レンダリングと更新はメインスレッドで行う前提でスレッドセーフではない。
/// </summary>

class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();
	// 初期化
	void Initialize(ObJect3dCommon* object3dCommon, Vector3 position);
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	//弾発射
	void Fire();
	//発射タイマー初期化
	void FireTime();
	//移動切り替え
	void MoveTime();
	// プレイヤーのワールド位置取得
	void GetWorldPosition(float& x, float& y, float& z) const override;
	// 当たり判定
	void OnCollision() override;
	// 敵の弾リスト取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }
	// 敵の生死判定
	bool IsDead() const override { return isDead_; }
	// プレイヤー情報セット
	void setPlayer(Player* player) { player_ = player; }
	// パーティクルマネージャーセット
	void SetParticleManager(ParticleManager* mgr) { particleManager_ = mgr; }
	//色変え
	void ChangeColor();

	//Stateパターン連携用API
	// State 切替要求
	void RequestStateChange(std::unique_ptr<EnemyState> newState) { pendingState_ = std::move(newState); }

	// Stateが利用する簡易API
	Vector3 GetPosition() const { return position_; }
	void SetPosition(const Vector3& p) { position_ = p; }

	void SetMove(const Vector3& m) { move = m; }
	Vector3 GetMove() const { return move; }

	Player* GetPlayer() const { return player_; }

	float GetSpeed() const { return speed; }
	void SetSpeed(float s) { speed = s; }

	// 行動タイマー操作
	int GetBehaviorTimer() const { return behaviorTimer_; }
	void SetBehaviorTimer(int v) { behaviorTimer_ = v; }
	void AddBehaviorTimer(int v) { behaviorTimer_ += v; }

	// dash/sine 用
	void SetDashTimer(int v) { dashTimer_ = v; }
	int GetDashTimer() const { return dashTimer_; }
	void AddDashTimer(int v) { dashTimer_ += v; }

	void SetSinePhase(float p) { sinePhase_ = p; }
	float GetSinePhase() const { return sinePhase_; }

	// State から呼ぶための弾発射ラッパー
	void FireSpreadPublic(int count, float totalAngleDeg) { FireSpread(count, totalAngleDeg); }
	void FireBurstPublic(int count) { FireBurst(count); }

	float GetBaseSpeed() const { return baseSpeed_; }

private:
	// 行動状態
	enum class BehaviorType {
		Patrol,
		Chase,
		SineWave,
		Dash,
		SpreadAttack,
		BurstAttack,
		Idle
	};

	// 基盤
	ObJect3dCommon* object3dCommon_ = nullptr;
	//敵の3Dモデル
	Transform modelTransform_;
	Object3d* Model_ = nullptr; // 3Dオブジェクト
	Vector3 position_; // 位置
	Vector3 rotation_; // 回転
	Vector3 scale_; // 拡大縮小
	float speed = 0.1f; // 移動速度
	float baseSpeed_ = 0.1f;
	//弾
	std::list<EnemyBullet*> bullets_;
	int Time = 0; //弾発射間隔用タイマー
	Vector3 bulletVel = { 0.0f,0.0f,0.0f };
	// 敵のHP
	float EnemyHp = 5.0f;
	//死亡フラグ
	bool isDead_ = false;
	//プレイヤー情報
	Player* player_ = nullptr;
	//移動
	Vector3 move = { 0.1f,0.0f,0.0f };
	int32_t moveTime = 0;

	// 行動関連
	BehaviorType behavior_ = BehaviorType::Patrol;
	int behaviorTimer_ = 0;
	int dashTimer_ = 0;
	float sinePhase_ = 0.0f;

	// 行動パラメータ
	static constexpr float kDashSpeed = 2.5f;
	static constexpr int kDashDuration = 30;
	static constexpr float kSineAmplitude = 0.6f;
	static constexpr float kSineFrequency = 0.15f;
	static constexpr int kBurstCount = 3;
	static constexpr int kSpreadCount = 5;
	static constexpr float kSpreadAngleDeg = 60.0f;

	//パーティクル
	ParticleManager* particleManager_ = nullptr;
	Vector3 particleVel = { 0.0f,0.0f,0.0f };
	int particleTimer_ = 0;

	// 被弾点滅用タイマー
	int flashTimer_ = 0;
	bool flashFlag_ = false; //点滅中フラグ
	// 点滅で色を切り替える間隔カウンタ
	int flashToggleCounter_ = 0;

	// 元の色を保持して復帰するための値
	Vector4 originalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 行動別の射撃サポート
	void FireSpread(int count, float totalAngleDeg);
	void FireBurst(int count);

	// 敵が死亡した際の爆発処理
	void Explode();
	// 爆発トリガーが二度実行されないようにするフラグ
	bool hasExploded_ = false;

	bool bulletActive = false; //弾発射フラグ

	static const int kFireInterval = 60; //弾の間隔
	static const int kMoveInterval = 360; //移動切り替え

	static const int kFlashDuration = 10; // 点滅時間
	// 点滅を何回繰り返すか
	static const int kFlashRepeat = 4;

	// State パターン用メンバ
	std::unique_ptr<EnemyState> state_;
	std::unique_ptr<EnemyState> pendingState_;
};
