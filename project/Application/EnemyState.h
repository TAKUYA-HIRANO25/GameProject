#pragma once
#include <memory>

class Enemy;

// State の抽象インターフェイス
class EnemyState {
public:
	virtual ~EnemyState() = default;
	// 状態に入ったときに呼ぶ
	virtual void Enter(Enemy* enemy) {}
	// 毎フレームの更新
	virtual void Update(Enemy* enemy) = 0;
	// 状態を抜けるときに呼ぶ
	virtual void Exit(Enemy* enemy) {}
	// 衝突時のイベントハンドラ
	virtual void OnCollision(Enemy* enemy) {}
	// 発射要求（Enemy のタイマー満了時に呼ばれる。既存の Enemy::Fire の代替）
	virtual void OnFire(Enemy* enemy) {}
};

// State ファクトリ関数（必要な State を追加する）
std::unique_ptr<EnemyState> CreatePatrolState();
std::unique_ptr<EnemyState> CreateChaseState();
std::unique_ptr<EnemyState> CreateSineWaveState();
std::unique_ptr<EnemyState> CreateDashState();
std::unique_ptr<EnemyState> CreateSpreadAttackState();
std::unique_ptr<EnemyState> CreateBurstAttackState();
std::unique_ptr<EnemyState> CreateIdleState();