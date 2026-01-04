#include "Enemy.h"
#include "Player.h"

// Enemy:
// - 敵の生成・更新・描画・弾発射・被弾エフェクト等を担当するクラス実装。
// - このファイルではオブジェクトのライフサイクル管理（new/delete）や
//   当たり判定時の色点滅・パーティクル生成ロジックを扱う。

Enemy::Enemy()
{
	// コンストラクタ: メンバは Initialize でセットアップする想定
}

Enemy::~Enemy()
{
	// デストラクタ: 保持しているモデルと弾を解放
	delete Model_;
	// bullets_ 内の EnemyBullet オブジェクトを全て削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		delete bullet;
		return true;
		});
}

// 初期化: object3d 共通・初期位置・モデル・HP・フラグ等を設定
void Enemy::Initialize(ObJect3dCommon* object3dCommon, Vector3 position)
{
	// 3D 共通参照を保持
	object3dCommon_ = object3dCommon;
	// 敵のワールド位置を保存
	position_ = position;
	// モデル生成・初期化
	Model_ = new Object3d();
	Model_->Initialize(object3dCommon);
	Model_->SetModel("Enemy/Enemy.obj");
	Model_->SetRotate({ 0.0f,3.14f,0.0f });
	Model_->SetTranslate(position);
	// 初期色を保存（被弾後の復帰用）
	originalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	// HP 初期値
	EnemyHp = 5.0f;
	// 死亡フラグ
	isDead_ = false;
	// タイマー初期化
	FireTime();
	MoveTime();

	// bullets_ を念のためクリア（初期化時の安全策）
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

	// 被弾点滅（ChangeColor 内で Model_->SetDiffuseColor を切り替える）
	ChangeColor();

	// 移動カウントダウン処理。moveTime が 0 で方向反転
	moveTime--;
	if (moveTime == 0) {
		move.x *= -1;
		moveTime = kMoveInterval;
	}
	// 速度を位置に適用
	position_ += move;

	// 発射カウントダウン
	Time--;
	if (Time == 0) {
		Time = kFireInterval;
		Fire();
	}

	// 所持弾の Update を呼び出す
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// HP が尽きたら死亡フラグを立てる
	if (EnemyHp <= 0) {
		isDead_ = true;
	}

	// モデルに位置を反映して更新
	Model_->SetTranslate(position_);
	Model_->Updata();
}

// 描画:
// - 死んでいない場合はモデルを描画、所有弾も描画
void Enemy::Draw()
{
	if (isDead_ == false) 
	{
		Model_->Draw();
	}
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw();
	}
}

// 弾を発射する処理:
// - プレイヤー方向へ向かう弾を生成して bullets_ に push_back する
void Enemy::Fire() {

	bulletActive = true;
	const float kBulletSpeed = -0.5f;
	bulletVel = { 0, 0, 0 };

	// 弾の軌道: プレイヤー方向へ向かう正規化ベクトルを求める
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

// 発射タイマー初期化
void Enemy::FireTime()
{
	Time = kFireInterval;
}

// 移動タイマー初期化
void Enemy::MoveTime()
{
	moveTime = 180;
}

// ワールド位置取得（現在 position_ を返す）
Vector3 Enemy::GetWorldPosition()
{
	Vector3 worldPos;
	worldPos = position_;

	return worldPos;
}

// 当たり判定時の処理:
// - HP 減少、パーティクル生成、点滅（色変更）を行う
void Enemy::OnCollision() {

	EnemyHp -= 1;

	// --- パーティクル生成（被弾エフェクト） ---
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

			// 点滅（赤）を開始：複数回トグルする実装
			if (Model_) {
				// 元色は Initialize 時に originalColor_ に保持しているので利用
				flashTimer_ = kFlashDuration * kFlashRepeat * 2;
				flashToggleCounter_ = kFlashDuration;

				// まず赤にする
				Model_->SetDiffuseColor({ 1.0f, 0.25f, 0.25f, 1.0f });
			}

			// パーティクルを spawn
			particleManager_->Spawn(spawnBase, vel, 30, "Particle.obj", { 0.8f,0.8f,0.8f });
		}
	}
}

// 色の点滅処理:
// - flashTimer_ と flashToggleCounter_ を用いて赤/元色を切り替える
void Enemy::ChangeColor()
{
	// --- 点滅処理 ---
	if (flashTimer_ > 0 && Model_) {
		// フレームを消費
		--flashTimer_;
		--flashToggleCounter_;

		// 一定フレーム毎に色を切り替える
		if (flashToggleCounter_ <= 0) {
			// トグル間隔をリセット
			flashToggleCounter_ = kFlashDuration;

			// 残りのトグル回数で赤/元色を切り替える（簡易実装）
			int remainingToggles = (flashTimer_ + kFlashDuration - 1) / kFlashDuration; // ceil
			if (remainingToggles % 2 == 0) {
				// 偶数なら赤（被弾色）
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

