#include "Enemy.h"
#include "Player.h"
#include <cmath>

Enemy::Enemy()
{
	// Initializerで初期化
}

Enemy::~Enemy()
{
	// デストラクタ:保持しているモデルと弾を解放
	delete Model_;
	// bullets_ 内のEnemyBulletオブジェクトを全て削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		delete bullet;
		return true;
		});
}

// 初期化:object3d 共通・初期位置・モデル・HP・フラグ等を設定
void Enemy::Initialize(ObJect3dCommon* object3dCommon, Vector3 position)
{
	// 3D共通参照を保持
	object3dCommon_ = object3dCommon;
	// 敵のワールド位置を保存
	position_ = position;
	// モデル生成・初期化
	Model_ = new Object3d();
	Model_->Initialize(object3dCommon);
	Model_->SetModel("Enemy/Enemy.obj");
	Model_->SetRotate({ 0.0f,3.14f,0.0f });
	Model_->SetTranslate(position);
	// 初期色を保存
	originalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	// HP 初期値
	EnemyHp = 5.0f;
	// 死亡フラグ
	isDead_ = false;
	// タイマー初期化
	FireTime();
	MoveTime();

	// 行動初期化
	behavior_ = BehaviorType::Patrol;
	behaviorTimer_ = kMoveInterval;

	// bulletsを念のためクリア
	bullets_.remove_if([](EnemyBullet* bullet) {
		delete bullet;
		return true;
		});
}

// 毎フレーム更新:
// - 弾の寿命チェック、色点滅処理、移動・発射処理、モデル更新を行う
void Enemy::Update()
{
	// 死亡した弾を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 被弾点滅
	ChangeColor();

	// 行動タイマー減算・切り替え
	behaviorTimer_--;
	if (behaviorTimer_ <= 0) {
		MoveTime(); //行動を切り替える
	}

	// 行動ごとの移動処理
	switch (behavior_) {
	case BehaviorType::Patrol:
		// 元のランダム左右移動
		moveTime--;
		if (moveTime == 0) {
			move.x *= -1;
			moveTime = kMoveInterval;
		}
		position_ += move;
		break;

	case BehaviorType::Chase:
		if (player_) {
			/*
			Vector3 dir = player_->GetWorldPosition() - position_;
			dir = MyMath::Normalize(dir);
			position_ += dir * speed;
			*/
			moveTime--;
			if (moveTime == 0) {
				move.x *= -1;
				moveTime = kMoveInterval;
			}
			position_ += move;
		}
		break;

	case BehaviorType::SineWave:
		// 前に進みつつ X 軸にサイン波オフセット
		/*
		sinePhase_ += kSineFrequency;
		position_.z += -speed; // 前に進む（z方向）
		position_.x += std::sin(sinePhase_) * kSineAmplitude;
		*/
		moveTime--;
		if (moveTime == 0) {
			move.x *= -1;
			moveTime = kMoveInterval;
		}
		position_ += move;
		break;

	case BehaviorType::Dash:
		if (dashTimer_ > 0) {
			/*
			// ダッシュ中はプレイヤー方向へ素早く接近
			if (player_) {
				Vector3 dir = player_->GetWorldPosition() - position_;
				dir = MyMath::Normalize(dir);
				position_ += dir * kDashSpeed;
			}
			*/
			moveTime--;
			if (moveTime == 0) {
				move.x *= -1;
				moveTime = kMoveInterval;
			}
			position_ += move;
			dashTimer_--;
		}
		else {
			// ノーマルに戻る
			moveTime--;
			if (moveTime == 0) {
				move.x *= -1;
				moveTime = kMoveInterval;
			}
			position_ += move;
		}
		break;

	case BehaviorType::Idle:
		// ほとんど動かない
		// small idle wiggle
		/*
		position_.x += std::sin(sinePhase_) * 0.02f;
		sinePhase_ += 0.05f;
		*/
		moveTime--;
		if (moveTime == 0) {
			move.x *= -1;
			moveTime = kMoveInterval;
		}
		position_ += move;
		break;

	default:
		moveTime--;
		if (moveTime == 0) {
			move.x *= -1;
			moveTime = kMoveInterval;
		}
		position_ += move;
		//position_ += move;
		break;
	}

	// 発射カウントダウン
	Time--;
	if (Time == 0) {
		Time = kFireInterval;
		Fire();
	}

	// 所持弾のUpdateを呼び出す
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// HP が尽きたら死亡フラグを立てる
	if (EnemyHp <= 0) {
		isDead_ = true;
	}
	// モデルに位置を反映して更新
	if (Model_) {
		Model_->SetTranslate(position_);
		Model_->Updata();
	}
}

// 描画:
void Enemy::Draw()
{
	if (isDead_ == false && Model_) 
	{
		Model_->Draw();
	}
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw();
	}
}

// 弾を発射する処理:行動に応じて通常弾/拡散/バーストを切り替える
void Enemy::Fire() {

	// デフォルトの単発追尾
	if (behavior_ == BehaviorType::SpreadAttack || behavior_ == BehaviorType::Chase) {
		FireSpread(kSpreadCount, kSpreadAngleDeg);
		return;
	}
	if (behavior_ == BehaviorType::BurstAttack || behavior_ == BehaviorType::Idle) {
		FireBurst(kBurstCount);
		return;
	}

	// 通常弾
	bulletActive = true;
	const float kBulletSpeed = -0.5f;
	bulletVel = { 0, 0, 0 };

	// 弾の軌道:プレイヤー方向へ向かう正規化ベクトルを求める
	Vector3 playerPosition = player_->GetWorldPosition();
	Vector3 enemyPosition = GetWorldPosition();
	Vector3 goalPosition = enemyPosition - playerPosition ;
	bulletVel = MyMath::Normalize(goalPosition);
	particleVel = bulletVel;
	bulletVel = { bulletVel.x * kBulletSpeed,bulletVel.y * kBulletSpeed, bulletVel.z * kBulletSpeed };

	// 弾オブジェクト生成と初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(object3dCommon_, Model_->GetTranslate(), bulletVel);
	
	// 所有コンテナに追加
	bullets_.push_back(newBullet);

}

// 拡散弾
void Enemy::FireSpread(int count, float totalAngleDeg)
{
	if (count <= 0) return;
	const float kBulletSpeed = -0.45f;

	// 基準方向
	Vector3 baseDir = MyMath::Normalize(GetWorldPosition() - player_->GetWorldPosition());

	// 中心から等間隔で角度を振る
	float half = totalAngleDeg * 0.5f;
	for (int i = 0; i < count; ++i) {
		float t = (count == 1) ? 0.0f : (float(i) / float(count - 1));
		float angleDeg = -half + t * totalAngleDeg;
		float angleRad = angleDeg * 3.14159265f / 180.0f;

		// XZ 平面で回転
		float cx = std::cos(angleRad);
		float sx = std::sin(angleRad);
		Vector3 dir;
		dir.x = baseDir.x * cx - baseDir.z * sx;
		dir.y = baseDir.y; // 高さ成分はそのまま
		dir.z = baseDir.x * sx + baseDir.z * cx;

		dir = MyMath::Normalize(dir);
		Vector3 vel = { dir.x * kBulletSpeed, dir.y * kBulletSpeed, dir.z * kBulletSpeed };

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(object3dCommon_, Model_->GetTranslate(), vel);
		bullets_.push_back(newBullet);
	}
	bulletActive = true;
	bool isGame = false;
}

// バースト
void Enemy::FireBurst(int count)
{
	if (count <= 0) return;
	const float kBulletSpeed = -0.55f;

	Vector3 baseDir = MyMath::Normalize(GetWorldPosition() - player_->GetWorldPosition());
	for (int i = 0; i < count; ++i) {
		// 少しランダムに拡散
		float rx = (std::rand() % 100 - 50) / 500.0f;
		float rz = (std::rand() % 100 - 50) / 500.0f;
		Vector3 dir = baseDir;
		dir.x += rx;
		dir.z += rz;
		dir = MyMath::Normalize(dir);
		Vector3 vel = { dir.x * kBulletSpeed, dir.y * kBulletSpeed, dir.z * kBulletSpeed };

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(object3dCommon_, Model_->GetTranslate(), vel);
		bullets_.push_back(newBullet);
	}
	bulletActive = true;
}

// 発射タイマー初期化
void Enemy::FireTime()
{
	Time = kFireInterval;
}

// 移動タイマー初期化
void Enemy::MoveTime()
{
	// 行動をランダムに選択
	int r = std::rand() % 6;
	switch (r) {
	case 0:
		behavior_ = BehaviorType::Patrol;
		behaviorTimer_ = kMoveInterval;
		break;
	case 1:
		behavior_ = BehaviorType::Chase;
		behaviorTimer_ = kMoveInterval;
		break;
	case 2:
		behavior_ = BehaviorType::SineWave;
		behaviorTimer_ = kMoveInterval;
		sinePhase_ = 0.0f;
		break;
	case 3:
		behavior_ = BehaviorType::Dash;
		behaviorTimer_ = kMoveInterval;
		dashTimer_ = kDashDuration;
		break;
	case 4:
		behavior_ = BehaviorType::SpreadAttack;
		behaviorTimer_ = kMoveInterval;
		break;
	case 5:
	default:
		behavior_ = BehaviorType::BurstAttack;
		behaviorTimer_ = kMoveInterval;
		break;
	}

	// 既存のmoveTimeもリセット
	moveTime = 180;
}

// ワールド位置取得
Vector3 Enemy::GetWorldPosition()
{
	Vector3 worldPos;
	worldPos = position_;

	return worldPos;
}

// 当たり判定時の処理:HP減少、パーティクル生成、点滅を行う
void Enemy::OnCollision() {

	EnemyHp -= 1;

	// パーティクル生成
	if (particleManager_) {
		const int kSpawn = 10;
		Vector3 spawnBase = Model_->GetTranslate();
		for (int i = 0; i < kSpawn; ++i) {
			// ランダムなオフセットと速度を付与
			float rx = (std::rand() % 100 - 50) / 150.0f; // -0.333 .. 0.333
			float ry = (std::rand() % 100 - 50) / 150.0f;
			float rz = (std::rand() % 50) / 150.0f + 0.2f; // 0.2 .. ~0.866
			Vector3 vel = { bulletVel.x * (0.4f + (std::rand() % 100) / 200.0f) + rx,
							bulletVel.y * (0.4f + (std::rand() % 100) / 200.0f) + ry,
							bulletVel.z * (0.4f + (std::rand() % 100) / 200.0f) + rz };
			vel *= -1.0f; // 敵なので下向きに飛ばす

			// 点滅を開始：複数回トグルする実装
			if (Model_ && flashFlag_ == false) {
				// 元色はInitialize時にoriginalColorに保持しているので利用
				flashTimer_ = kFlashDuration * kFlashRepeat * 2;
				flashToggleCounter_ = kFlashDuration;

				// まず赤にする
				Model_->SetDiffuseColor({ 1.0f, 0.25f, 0.25f, 1.0f });
			}

			// パーティクルを生成
			particleManager_->Spawn(spawnBase, vel, 30, "Particle.obj", { 0.8f,0.8f,0.8f });
		}
	}
}

// 色の点滅処理:
// - flashTimerとflashToggleCounterを用いて赤/元色を切り替える
void Enemy::ChangeColor()
{

	// 点滅処理
	if (flashTimer_ > 0 && Model_) {
		// フレームを消費
		--flashTimer_;
		--flashToggleCounter_;

		// 一定フレーム毎に色を切り替える
		if (flashToggleCounter_ <= 0) {
			// トグル間隔をリセット
			flashToggleCounter_ = kFlashDuration;

			// 残りのトグル回数で赤/元色を切り替える
			int remainingToggles = (flashTimer_ + kFlashDuration - 1) / kFlashDuration; // ceil
			if (remainingToggles % 2 == 0) {
				// 偶数なら赤
				Model_->SetDiffuseColor({0.8f, 0.0f, 0.0f, 1.0f });
			}
			else {
				// 奇数なら元の色に戻す
				Model_->SetDiffuseColor(originalColor_);
			}
		}

		// 点滅終了時に確実に元色へ復帰
		if (flashTimer_ == 0) {
			Model_->SetDiffuseColor(originalColor_);
		}
	}
}

