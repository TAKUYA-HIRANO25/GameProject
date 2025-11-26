#include "Enemy.h"
#include "Player.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
	delete Model_;
	bullets_.remove_if([](EnemyBullet* bullet) {
		delete bullet;
		return true;
		});
}

void Enemy::Initialize(ObJect3dCommon* object3dCommon, Vector3 position)
{
	//基盤
	object3dCommon_ = object3dCommon;
	//ポジション
	position_ = position;
	//モデル
	Model_ = new Object3d();
	Model_->Initialize(object3dCommon);
	Model_->SetModel("box.obj");
	Model_->SetTranslate(position);
	//HP
	EnemyHp = 5.0f;
	//死亡フラグ
	isDead_ = false;
	//初期設定
	FireTime();
	MoveTime();

	bullets_.remove_if([](EnemyBullet* bullet) {
		delete bullet;
		return true;
	});
}

void Enemy::Update()
{
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	moveTime--;
	if (moveTime == 0) {
		move.x *= -1;
		moveTime = kMoveInterval;
	}
	position_ += move;

	Time--;
	if (Time == 0) {
		Time = kFireInterval;
		Fire();
	}

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	if (EnemyHp <= 0) {
		isDead_ = true;
	}

	Model_->SetTranslate(position_);

	Model_->Updata();
}

void Enemy::Draw()
{
	Model_->Draw();
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw();
	}
}


void Enemy::Fire() {

	bulletActive = true;
	const float kBulletSpeed = -0.5f;
	bulletVel = { 0, 0, 0 };

	//弾の軌道
	Vector3 playerPosition = player_->GetWorldPosition();
	Vector3 enemyPosition = GetWorldPosition();
	Vector3 goalPosition = enemyPosition - playerPosition ;
	bulletVel = MyMath::Normalize(goalPosition);
	particleVel = bulletVel;
	bulletVel = { bulletVel.x * kBulletSpeed,bulletVel.y * kBulletSpeed, bulletVel.z * kBulletSpeed };
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(object3dCommon_, Model_->GetTranslate(), bulletVel);
	
	bullets_.push_back(newBullet);

}

void Enemy::FireTime()
{
	Time = kFireInterval;
}

void Enemy::MoveTime()
{
	moveTime = 180;
}

Vector3 Enemy::GetWorldPosition()
{
	Vector3 worldPos;
	worldPos = position_;

	return worldPos;
}

void Enemy::OnCollision() {

	EnemyHp -= 1;

	// --- ここからパーティクル生成（発射エフェクト） ---
	if (particleManager_) {
		const int kSpawn = 10;
		Vector3 spawnBase = Model_->GetTranslate();
		for (int i = 0; i < kSpawn; ++i) {
			// 少しランダムなオフセットと速度
			float rx = (std::rand() % 100 - 50) / 150.0f; // -0.333 .. 0.333
			float ry = (std::rand() % 100 - 50) / 150.0f;
			float rz = (std::rand() % 50) / 150.0f + 0.2f; // 0.2 .. ~0.866
			Vector3 vel = { bulletVel.x * (0.4f + (std::rand() % 100) / 200.0f) + rx,
							bulletVel.y * (0.4f + (std::rand() % 100) / 200.0f) + ry,
							bulletVel.z * (0.4f + (std::rand() % 100) / 200.0f) + rz };
			vel *= -1.0f; // 敵なので下向きに飛ばす
			particleManager_->Spawn(spawnBase, vel, 30, "Particle.obj", { 0.8f,0.8f,0.8f });
		}
	}
}

