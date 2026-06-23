#include "EnemyState.h"
#include "Enemy.h"
#include <cmath>

// 各 State の持続時間（フレーム）
static constexpr int kPatrolDuration  = 300;
static constexpr int kChaseDuration   = 240;
static constexpr int kSineDuration    = 240;
static constexpr int kDashDuration    = 60;
static constexpr int kSpreadDuration  = 30;
static constexpr int kBurstDuration   = 30;
static constexpr int kIdleDuration    = 60;

// Patrol
class PatrolState : public EnemyState {
public:
	void Enter(Enemy* enemy) override {
		enemy->SetBehaviorTimer(0);
		// 初期移動をゼロにするか既存moveを保持するか選べる
		enemy->SetMove({ 0.02f * enemy->GetSpeed(), 0.0f, 0.0f });
	}
	void Update(Enemy* enemy) override {
		// 左右往復をサインで作る
		int t = enemy->GetBehaviorTimer();
		float phase = std::sin(t * 0.05f);
		Vector3 move = { phase * 0.02f * enemy->GetSpeed(), 0.0f, 0.0f };
		enemy->SetMove(move);

		// 位置に反映
		Vector3 pos = enemy->GetPosition();
		pos.x += move.x;
		pos.y += move.y;
		pos.z += move.z;
		enemy->SetPosition(pos);

		enemy->AddBehaviorTimer(1);

		// 所定時間でChaseに遷移
		if (enemy->GetBehaviorTimer() > kPatrolDuration) {
			enemy->RequestStateChange(CreateChaseState());
		}
	}
};

// Chase
class ChaseState : public EnemyState {
public:
	void Enter(Enemy* enemy) override {
		enemy->SetBehaviorTimer(0);
	}
	void Update(Enemy* enemy) override {
		auto* player = enemy->GetPlayer();
		if (!player) {
			// プレイヤー不在ならIdleに遷移
			enemy->RequestStateChange(CreateIdleState());
			return;
		}

		// プレイヤー方向へ移動
		Vector3 ppos = player->GetPosition();
		Vector3 epos = enemy->GetPosition();
		Vector3 dir = { ppos.x - epos.x, ppos.y - epos.y, ppos.z - epos.z };
		// 正規化
		float len = std::sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
		if (len > 1e-6f) {
			dir.x /= len; dir.y /= len; dir.z /= len;
		}
		Vector3 move = { dir.x * enemy->GetSpeed(), dir.y * enemy->GetSpeed(), dir.z * enemy->GetSpeed() };

		// 位置に反映
		epos.x += move.x;
		epos.y += move.y;
		epos.z += move.z;
		enemy->SetPosition(epos);

		// 任意の射撃
		enemy->FireBurstPublic(1);

		enemy->AddBehaviorTimer(1);

		// 所定時間でSineWaveに遷移
		if (enemy->GetBehaviorTimer() > kChaseDuration) {
			enemy->RequestStateChange(CreateSineWaveState());
		}
	}
};

// --- SineWave ---
class SineWaveState : public EnemyState {
public:
	void Enter(Enemy* enemy) override {
		enemy->SetBehaviorTimer(0);
		enemy->SetSinePhase(0.0f);
	}
	void Update(Enemy* enemy) override {
		float phase = enemy->GetSinePhase();
		phase += 0.15f;
		enemy->SetSinePhase(phase);

		float offsetX = std::sin(phase) * 0.6f;
		Vector3 move = { offsetX * 0.02f * enemy->GetSpeed(), 0.0f, 0.0f };

		// 位置に反映
		Vector3 pos = enemy->GetPosition();
		pos.x += move.x;
		pos.y += move.y;
		pos.z += move.z;
		enemy->SetPosition(pos);

		enemy->AddBehaviorTimer(1);

		// 所定時間でDashに遷移
		if (enemy->GetBehaviorTimer() > kSineDuration) {
			enemy->RequestStateChange(CreateDashState());
		}
	}
};

// --- Dash ---
class DashState : public EnemyState {
public:
	void Enter(Enemy* enemy) override {
		enemy->SetBehaviorTimer(0);
		prevSpeed_ = enemy->GetSpeed();
		enemy->SetSpeed(prevSpeed_ * kDashMultiplier);
		// dash開始時に短めの前進を与える
	}
	void Exit(Enemy* enemy) override {
		// 速度を元に戻す
		enemy->SetSpeed(prevSpeed_);
	}
	void Update(Enemy* enemy) override {
		// 単純に前方へ高速移動させる
		Vector3 pos = enemy->GetPosition();
		pos.z += -enemy->GetSpeed(); 
		enemy->SetPosition(pos);

		enemy->AddBehaviorTimer(1);

		// 一定時間でSpreadAttackに遷移
		if (enemy->GetBehaviorTimer() > kDashDuration) {
			enemy->RequestStateChange(CreateSpreadAttackState());
		}
	}
private:
	float prevSpeed_ = 0.0f;
	static constexpr float kDashMultiplier = 2.5f;
};

// SpreadAttack
class SpreadAttackState : public EnemyState {
public:
	void Enter(Enemy* enemy) override {
		enemy->SetBehaviorTimer(0);
		// 一度だけ拡散発射
		enemy->FireSpreadPublic(5, 60.0f);
	}
	void Update(Enemy* enemy) override {
		// 少しウロウロして時間経過で次へ
		Vector3 pos = enemy->GetPosition();
		// 小さく左右にランダム移動
		float rx = ((std::rand() % 100) - 50) / 500.0f;
		pos.x += rx * 0.5f;
		enemy->SetPosition(pos);

		enemy->AddBehaviorTimer(1);

		if (enemy->GetBehaviorTimer() > kSpreadDuration) {
			enemy->RequestStateChange(CreateBurstAttackState());
		}
	}
};

// --- BurstAttack ---
class BurstAttackState : public EnemyState {
public:
	void Enter(Enemy* enemy) override {
		enemy->SetBehaviorTimer(0);
		// 一度だけバースト発射
		enemy->FireBurstPublic(3);
	}
	void Update(Enemy* enemy) override {
		// 待機してから次へ
		enemy->AddBehaviorTimer(1);
		if (enemy->GetBehaviorTimer() > kBurstDuration) {
			enemy->RequestStateChange(CreateIdleState());
		}
	}
};

// Idle
class IdleState : public EnemyState {
public:
	void Enter(Enemy* enemy) override {
		enemy->SetBehaviorTimer(0);
	}
	void Update(Enemy* enemy) override {
		// 停止
		// 位置はそのままにする
		enemy->AddBehaviorTimer(1);
		if (enemy->GetBehaviorTimer() > kIdleDuration) {
			enemy->RequestStateChange(CreatePatrolState());
		}
	}
};

// Factory 実装
std::unique_ptr<EnemyState> CreatePatrolState() { return std::make_unique<PatrolState>(); }
std::unique_ptr<EnemyState> CreateChaseState() { return std::make_unique<ChaseState>(); }
std::unique_ptr<EnemyState> CreateSineWaveState() { return std::make_unique<SineWaveState>(); }
std::unique_ptr<EnemyState> CreateDashState() { return std::make_unique<DashState>(); }
std::unique_ptr<EnemyState> CreateSpreadAttackState() { return std::make_unique<SpreadAttackState>(); }
std::unique_ptr<EnemyState> CreateBurstAttackState() { return std::make_unique<BurstAttackState>(); }
std::unique_ptr<EnemyState> CreateIdleState() { return std::make_unique<IdleState>(); }