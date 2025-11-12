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
	EnemyHp = 10.0f;
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
	const float kBulletSpeed = 0.5f;
	Vector3 velocity(0, 0, 0);

	//弾の軌道
	Vector3 playerPosition = player_->GetWorldPosition();
	Vector3 enemyPosition = GetWorldPosition();
	Vector3 goalPosition = playerPosition - enemyPosition;
	velocity = MyMath::Normalize(goalPosition);
	velocity = { velocity.x * kBulletSpeed,velocity.y * kBulletSpeed, velocity.z * kBulletSpeed };
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(object3dCommon_, Model_->GetTranslate(), velocity);
	
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

}

