#pragma once
#include <memory>
#include <list>
#include "GameObject.h"
#include "Bullet.h"
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
	void Initialize(ObJect3dCommon* object3dCommon, Vector3 position);
	void Update() override;
	void Draw() override;
	void Fire();
	void FireTime();
	void MoveTime();
	void GetWorldPosition(float& x, float& y, float& z) const override;
	void OnCollision() override;
	// 戻り型を unique_ptr のコンテナ参照に変更
	const std::list<std::unique_ptr<Bullet>>& GetBullets() const { return bullets_; }
	bool IsDead() const override { return isDead_; }
	void setPlayer(Player* player) { player_ = player; }
	void SetParticleManager(ParticleManager* mgr) { particleManager_ = mgr; }
	void ChangeColor();

	void RequestStateChange(std::unique_ptr<EnemyState> newState) { pendingState_ = std::move(newState); }

	Vector3 GetPosition() const { return position_; }
	void SetPosition(const Vector3& p) { position_ = p; }

	void SetMove(const Vector3& m) { move = m; }
	Vector3 GetMove() const { return move; }

	Player* GetPlayer() const { return player_; }

	float GetSpeed() const { return speed; }
	void SetSpeed(float s) { speed = s; }

	int GetBehaviorTimer() const { return behaviorTimer_; }
	void SetBehaviorTimer(int v) { behaviorTimer_ = v; }
	void AddBehaviorTimer(int v) { behaviorTimer_ += v; }

	// dash/sine 用
	void SetDashTimer(int v) { dashTimer_ = v; }
	int GetDashTimer() const { return dashTimer_; }
	void AddDashTimer(int v) { dashTimer_ += v; }

	void SetSinePhase(float p) { sinePhase_ = p; }
	float GetSinePhase() const { return sinePhase_; }

	// Stateから呼ぶための弾発射ラッパー
	void FireSpreadPublic(int count, float totalAngleDeg) { FireSpread(count, totalAngleDeg); }
	void FireBurstPublic(int count) { FireBurst(count); }

	float GetBaseSpeed() const { return baseSpeed_; }

	// 行動パラメータにアクセスするためのアクセサ（定数は private のまま外部参照を許可）
	static inline float GetDashSpeed() { return kDashSpeed; }
	static inline int   GetDashDuration() { return kDashDuration; }
	static inline float GetSineAmplitude() { return kSineAmplitude; }
	static inline float GetSineFrequency() { return kSineFrequency; }
	static inline int   GetBurstCount() { return kBurstCount; }
	static inline int   GetSpreadCount() { return kSpreadCount; }
	static inline float GetSpreadAngleDeg() { return kSpreadAngleDeg; }
	static inline int   GetFireInterval() { return kFireInterval; }
	static inline int   GetMoveInterval() { return kMoveInterval; }

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

	ObJect3dCommon* object3dCommon_ = nullptr;
	Transform modelTransform_;
	// スマートポインタ化
	std::unique_ptr<Object3d> Model_{};
	Vector3 position_;
	Vector3 rotation_;
	Vector3 scale_;
	float speed = 0.1f;
	float baseSpeed_ = 0.1f;
	// 弾はunique_ptr 
	std::list<std::unique_ptr<Bullet>> bullets_;
	int Time = 0;
	Vector3 bulletVel = { 0.0f,0.0f,0.0f };
	float EnemyHp = 5.0f;
	bool isDead_ = false;
	Player* player_ = nullptr;
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
