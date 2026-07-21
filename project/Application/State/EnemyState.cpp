#include "EnemyState.h"
#include "Enemy.h"
#include "Player.h"
#include <cmath>
#include <cstdlib>

class PatrolState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		// デフォルト移動
		e->SetBehaviorTimer(Enemy::GetMoveInterval());
		// move が適切に初期化済みであればそのまま使う
	}
	void Update(Enemy* e) override {
		// 単純な左右移動
		Vector3 pos = e->GetPosition();
		Vector3 mv = e->GetMove();
		pos.x += mv.x;
		e->SetPosition(pos);

		// 行動タイマーを減算し、0なら次状態へ委譲
		e->AddBehaviorTimer(-1);
		if (e->GetBehaviorTimer() <= 0) {
			e->MoveTime(); 
		}
	}
};


class ChaseState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(Enemy::GetMoveInterval());
	}
	void Update(Enemy* e) override {
		Player* p = e->GetPlayer();
		if (p) {
			float px, py, pz;
			p->GetWorldPosition(px, py, pz);
			Vector3 pos = e->GetPosition();
			// 単純にプレイヤー方向へ移動
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
	// Chaseでは拡散弾モードで撃つ仕様だったのでOnFireを実装
	void OnFire(Enemy* e) override {
		e->FireSpreadPublic(Enemy::GetSpreadCount(), Enemy::GetSpreadAngleDeg());
	}
};

class SineWaveState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(Enemy::GetMoveInterval());
		e->SetSinePhase(0.0f);
	}
	void Update(Enemy* e) override {
		Vector3 pos = e->GetPosition();
		float phase = e->GetSinePhase();
		phase += Enemy::GetSineFrequency();
		pos.y += std::sin(phase) * Enemy::GetSineAmplitude();
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


class DashState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		// ダッシュ時間の設定
		e->SetBehaviorTimer(Enemy::GetMoveInterval());
		e->SetDashTimer(Enemy::GetDashDuration());
	}
	void Update(Enemy* e) override {
		Vector3 pos = e->GetPosition();
		Vector3 mv = e->GetMove();
		// ダッシュ中は高速移動、それ以外は通常移動
		if (e->GetDashTimer() > 0) {
			pos.x += mv.x * Enemy::GetDashSpeed();
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

class SpreadAttackState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(Enemy::GetMoveInterval());
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
		e->FireSpreadPublic(Enemy::GetSpreadCount(), Enemy::GetSpreadAngleDeg());
	}
};

class BurstAttackState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(Enemy::GetMoveInterval());
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
		e->FireBurstPublic(Enemy::GetBurstCount());
	}
};

class IdleState : public EnemyState {
public:
	void Enter(Enemy* e) override {
		e->SetBehaviorTimer(Enemy::GetMoveInterval());
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
		e->FireBurstPublic(Enemy::GetBurstCount());
	}
};

// ファクトリ関数の実装
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