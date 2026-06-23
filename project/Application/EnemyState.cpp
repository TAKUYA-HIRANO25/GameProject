#include "EnemyState.h"
#include "Enemy.h"
#include "Player.h"
#include <cmath>
#include <cstdlib>

// 注意: Enemy の内部定数は private のため、一部値をここで再定義しています。
// 必要なら Enemy に public な定数アクセスを追加してください。
static constexpr int kMoveInterval_Local = 360;
static constexpr int kDashDuration_Local = 30;
static constexpr float kSineAmplitude_Local = 0.6f;
static constexpr float kSineFrequency_Local = 0.15f;
static constexpr int kBurstCount_Local = 3;
static constexpr int kSpreadCount_Local = 5;
static constexpr float kSpreadAngleDeg_Local = 60.0f;
static constexpr float kDashSpeed_Local = 2.5f;

//
// PatrolState
//
class PatrolState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		// デフォルト移動
		e->SetBehaviorTimer(kMoveInterval_Local);
		// move が適切に初期化済みであればそのまま使う
	}
	void Update(Enemy* e) override {
		// 単純な左右移動（既存の move を利用）
		Vector3 pos = e->GetPosition();
		Vector3 mv = e->GetMove();
		pos.x += mv.x;
		e->SetPosition(pos);

		// 行動タイマーを減算し、0 なら次状態へ委譲（Enemy::MoveTime を用いる）
		e->AddBehaviorTimer(-1);
		if (e->GetBehaviorTimer() <= 0) {
			e->MoveTime(); // MoveTime は状態を RequestStateChange するように修正済み
		}
	}
};

//
// ChaseState
//
class ChaseState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(kMoveInterval_Local);
	}
	void Update(Enemy* e) override {
		Player* p = e->GetPlayer();
		if (p) {
			float px, py, pz;
			p->GetWorldPosition(px, py, pz);
			Vector3 pos = e->GetPosition();
			// 単純にプレイヤー方向へ移動（速度は baseSpeed を利用）
			Vector3 dir = { pos.x - px, pos.y - py, pos.z - pz };
			dir = MyMath::Normalize(dir);
			Vector3 newPos = pos;
			float sp = e->GetSpeed();
			newPos.x += dir.x * sp;
			newPos.y += dir.y * sp;
			newPos.z += dir.z * sp;
			e->SetPosition(newPos);
		}

		e->AddBehaviorTimer(-1);
		if (e->GetBehaviorTimer() <= 0) {
			e->MoveTime();
		}
	}
	// Chase では拡散弾モードで撃つ仕様だったので OnFire を実装
	void OnFire(Enemy* e) override {
		e->FireSpreadPublic(kSpreadCount_Local, kSpreadAngleDeg_Local);
	}
};

//
// SineWaveState
//
class SineWaveState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(kMoveInterval_Local);
		e->SetSinePhase(0.0f);
	}
	void Update(Enemy* e) override {
		Vector3 pos = e->GetPosition();
		float phase = e->GetSinePhase();
		phase += kSineFrequency_Local;
		pos.y += std::sin(phase) * kSineAmplitude_Local;
		Vector3 mv = e->GetMove();
		pos.x += mv.x;
		e->SetPosition(pos);
		e->SetSinePhase(phase);

		e->AddBehaviorTimer(-1);
		if (e->GetBehaviorTimer() <= 0) {
			e->MoveTime();
		}
	}
};

//
// DashState
//
class DashState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		// ダッシュ時間の設定
		e->SetBehaviorTimer(kMoveInterval_Local);
		e->SetDashTimer(kDashDuration_Local);
	}
	void Update(Enemy* e) override {
		Vector3 pos = e->GetPosition();
		Vector3 mv = e->GetMove();
		// ダッシュ中は高速移動、それ以外は通常移動
		if (e->GetDashTimer() > 0) {
			pos.x += mv.x * kDashSpeed_Local;
			e->AddDashTimer(-1);
		}
		else {
			pos.x += mv.x;
		}
		e->SetPosition(pos);

		e->AddBehaviorTimer(-1);
		if (e->GetBehaviorTimer() <= 0) {
			e->MoveTime();
		}
	}
};

//
// SpreadAttackState
//
class SpreadAttackState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(kMoveInterval_Local);
	}
	void Update(Enemy* e) override {
		// 移動はパトロール風
		Vector3 pos = e->GetPosition();
		Vector3 mv = e->GetMove();
		pos.x += mv.x;
		e->SetPosition(pos);

		e->AddBehaviorTimer(-1);
		if (e->GetBehaviorTimer() <= 0) {
			e->MoveTime();
		}
	}
	void OnFire(Enemy* e) override {
		e->FireSpreadPublic(kSpreadCount_Local, kSpreadAngleDeg_Local);
	}
};

//
// BurstAttackState
//
class BurstAttackState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(kMoveInterval_Local);
	}
	void Update(Enemy* e) override {
		Vector3 pos = e->GetPosition();
		Vector3 mv = e->GetMove();
		pos.x += mv.x;
		e->SetPosition(pos);

		e->AddBehaviorTimer(-1);
		if (e->GetBehaviorTimer() <= 0) {
			e->MoveTime();
		}
	}
	void OnFire(Enemy* e) override {
		e->FireBurstPublic(kBurstCount_Local);
	}
};

//
// IdleState
//
class IdleState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(kMoveInterval_Local);
	}
	void Update(Enemy* e) override {
		// Idle は微小移動または停止
		Vector3 pos = e->GetPosition();
		Vector3 mv = e->GetMove();
		pos.x += mv.x * 0.2f; // ゆっくり
		e->SetPosition(pos);

		e->AddBehaviorTimer(-1);
		if (e->GetBehaviorTimer() <= 0) {
			e->MoveTime();
		}
	}
	void OnFire(Enemy* e) override {
		// Idle はバースト射撃指定とされている既存コード仕様に準拠
		e->FireBurstPublic(kBurstCount_Local);
	}
};

//
// ファクトリ実装
//
std::unique_ptr<EnemyState> CreatePatrolState() {
	return std::make_unique<PatrolState>();
}
std::unique_ptr<EnemyState> CreateChaseState() {
	return std::make_unique<ChaseState>();
}
std::unique_ptr<EnemyState> CreateSineWaveState() {
	return std::make_unique<SineWaveState>();
}
std::unique_ptr<EnemyState> CreateDashState() {
	return std::make_unique<DashState>();
}
std::unique_ptr<EnemyState> CreateSpreadAttackState() {
	return std::make_unique<SpreadAttackState>();
}
std::unique_ptr<EnemyState> CreateBurstAttackState() {
	return std::make_unique<BurstAttackState>();
}
std::unique_ptr<EnemyState> CreateIdleState() {
	return std::make_unique<IdleState>();
}