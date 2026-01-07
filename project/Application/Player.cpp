#include "Player.h"
#include "MatuilityForText.h"

Player::Player()
{
	// コンストラクタ: メンバは Initialize() で初期化する想定
}

Player::~Player()
{
	// デストラクタ: 所有する Model/reticleModel と弾を解放する
	delete Model_;
	delete reticleModel_;
	bulletList_.remove_if([](PlayerBullet* bullet) {
		delete bullet;
		return true;
		});
}

void Player::Initialize(ObJect3dCommon* object3dCommon, Input* input) {
	// 3D 共通参照を保持
	object3dCommon_ = object3dCommon;

	// プレイヤーモデル生成・初期化
	Model_ = new Object3d();
	Model_->Initialize(object3dCommon);
	Model_->SetModel("Player/Player.obj");
	Model_->SetTranslate(position_);

	spriteCommon_ = SpriteCommon::GetInstance();
	if (spriteCommon_) {
		reticleSprite_ = new Sprite();
		reticleSprite_->Initialize(spriteCommon_, "resources/Reticle.png");
		reticleSprite_->SetAnchorPoint({ 0.5f,0.5f });
		reticleSprite_->SetSize(Vector2(64,64));
		// 初期位置は画面中央
		reticleSprite_->SetPosition(Vector2(float(WinApp::kClientWidth) * 0.5f, float(WinApp::kClientHeight) * 0.5f));
	}

	// 初期色・ステータスを保存
	originalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	isDead_ = false;
	PlayerHP = 10.0f;

	// 入力はシングルトンから取得（引数 input が nullptr の場合でも安全に扱うため）
	input_ = Input::GetInstance();

	// bulletList_ を念のためクリア（初期化時の安全措置）
	bulletList_.remove_if([](PlayerBullet* bullet) {
		delete bullet;
		return true;
		});
}

void Player::Update()
{
	// 死亡した弾の解放
	bulletList_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	// 被弾時の点滅処理（色の切替）
	ChangeColor();

	// 入力に基づく移動処理
	Move();

	// マウス位置からレティクル位置を決定して更新
	Reticle();

	// 発射処理（スペースキー）
	Fire();

	// 所有弾の更新
	for (PlayerBullet* bullet : bulletList_) {
		bullet->Update();
	}

	// HP が 0 なら死亡フラグを立てる（必要なら <= 0 に変更）
	if (PlayerHP == 0) {
		isDead_ = true;
	}

	// モデルに位置を反映して行列更新（Object3d 側で World 行列等を作る想定）
	Model_->SetTranslate(position_);
	Model_->Updata();

}

void Player::Draw()
{
	if (isDead_ == false) {
		Model_->Draw();
	}

	for (PlayerBullet* bullet : bulletList_) {
		bullet->Draw();
	}
}

void Player::SpriteDraw() {
	if (isDead_ == false) {
		reticleSprite_->Draw();
	}
}

void Player::Move()
{
	Vector3 move = { 0,0,0 }; // 移動量
	const float kCharacterSpeed = 0.2f; // キャラクターの移動速度

	// 左右上下・前後移動（Q/E で Z 軸）
	if (input_->PushKey(DIK_A)) {
		move.x -= kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_D)) {
		move.x += kCharacterSpeed;
	}
	if (input_->PushKey(DIK_W)) {
		move.y += kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_S)) {
		move.y -= kCharacterSpeed;
	}

	if (input_->PushKey(DIK_Q)) {
		move.z += kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_E)) {
		move.z -= kCharacterSpeed;
	}

	// カメラの移動量を加算してワールド位置へ適用
	position_ += move + railCameraVelocity_;
}

void Player::Fire()
{

	if (input_->TriggerKey(DIK_SPACE) && bulletTime <= 0) {

		bulletActive = true;
		bulletTime = 10;

		const float kBulletSpeed = 1.0f;

		// 発射元の位置と方向を決定（reticleWorldPos_ を目標にする）
		Vector3 startPos = Model_->GetTranslate();
		dir = { 0.0f, 0.0f, 1.0f };
		dir = { reticleWorldPos_.x - startPos.x, reticleWorldPos_.y - startPos.y, reticleWorldPos_.z - startPos.z };
		dir = Normalize(dir);
		Vector3 velocity = { dir.x * kBulletSpeed, dir.y * kBulletSpeed, dir.z * kBulletSpeed };
		velocity += railCameraVelocity_; // カメラ移動を弾速に加味

		// 弾オブジェクト生成・初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(object3dCommon_, Model_->GetTranslate(), velocity);
		bulletList_.push_back(newBullet);
	}
	else {
		// 発射インターバルのカウントダウン（下限 0）
		bulletTime--;
		if (bulletTime <= 0) {
			bulletTime = 0;
		}
	}
}

Vector3 Player::GetWorldPosition()
{
	Vector3 worldPos;
	worldPos = position_;

	return worldPos;
}

void Player::OnCollision()
{
	PlayerHP -= 1;

	// パーティクルの生成（被弾エフェクト）
	if (particleManager_) {
		const int kSpawn = 10;
		Vector3 spawnBase = Model_->GetTranslate();
		for (int i = 0; i < kSpawn; ++i) {
			// ランダムなオフセットと速度を付与
			float rx = (std::rand() % 100 - 50) / 150.0f; // -0.333 .. 0.333
			float ry = (std::rand() % 100 - 50) / 150.0f;
			float rz = (std::rand() % 50) / 150.0f + 0.2f; // 0.2 .. ~0.866
			Vector3 vel = { dir.x * (0.4f + (std::rand() % 100) / 200.0f) + rx,
							dir.y * (0.4f + (std::rand() % 100) / 200.0f) + ry,
							dir.z * (0.4f + (std::rand() % 100) / 200.0f) + rz };
			particleManager_->Spawn(spawnBase, vel, 30, "Particle.obj", { 0.8f,0.8f,0.8f });
		}
	}

	// 被弾点滅を開始（flashTimer_ / flashToggleCounter_ を用いる）
	if (Model_) {
		flashTimer_ = flashDuration_ * flashRepeat_ * 2;
		flashToggleCounter_ = flashDuration_;
		Model_->SetDiffuseColor(flashColor_);
	}
}

void Player::Reticle()
{
	// 必要な参照が無ければ早期リターン
	if (!input_ || !object3dCommon_) return;
	Camera* camera = object3dCommon_->GetDefaultCamera();
	if (!camera) return;

	// クライアント座標（ピクセル）
	Vector2 cursor = input_->GetCursorClientPos2();
	const float width = static_cast<float>(WinApp::kClientWidth);
	const float height = static_cast<float>(WinApp::kClientHeight);
	// スプライト版レティクルのスクリーン座標更新
	reticleSprite_->SetPosition(cursor);
	reticleSprite_->Update();
	// NDC に変換
	float nx = (cursor.x / width) * 2.0f - 1.0f;
	float ny = -((cursor.y / height) * 2.0f - 1.0f); // 上下反転

	// 近クリップ・遠クリップの NDC を定義
	Vector3 ndcNear = { nx, ny, 0.0f };
	Vector3 ndcFar = { nx, ny, 1.0f };

	// カメラの逆行列を取得して NDC -> ワールド変換
	Matrix4x4 invProj = Inverse(camera->GetProjectionMatrix());
	Matrix4x4 invView = Inverse(camera->GetViewMatrix());

	Vector3 pNear = TransformS(ndcNear, invProj); // -> eye space (then w division)
	pNear = TransformS(pNear, invView);           // -> world space

	Vector3 pFar = TransformS(ndcFar, invProj);
	pFar = TransformS(pFar, invView);

	// レイを作成して正規化
	Vector3 rayDir = { pFar.x - pNear.x, pFar.y - pNear.y, pFar.z - pNear.z };
	rayDir = Normalize(rayDir);

	// レティクル位置はカメラ位置 + rayDir * 距離
	Vector3 camPos = camera->GetTranslate();
	reticleWorldPos_ = { camPos.x + rayDir.x * reticleDistance_, camPos.y + rayDir.y * reticleDistance_, camPos.z + rayDir.z * reticleDistance_ };

	// reticleModel_ にセット（描画時に Updata() しているためここでは SetTranslate のみ）
	if (reticleModel_) {
		reticleModel_->SetTranslate(reticleWorldPos_);
	}
}

// レールカメラの移動速度を受け取り、プレイヤーの動きに反映するために保持する。
void Player::SetRailCameraVelocity(Vector3 velocity)
{
	railCameraVelocity_ = velocity;
}

// ChangeColor:
// 被弾点滅ロジック。flashTimer_ と flashToggleCounter_ で赤/元色を切り替える。
// Model_->SetDiffuseColor() を使って表示色を変更する。
void Player::ChangeColor()
{
	if (flashTimer_ > 0 && Model_) {
		--flashTimer_;
		--flashToggleCounter_;

		if (flashToggleCounter_ <= 0) {
			flashToggleCounter_ = flashDuration_;

			int remainingToggles = (flashTimer_ + flashDuration_ - 1) / flashDuration_;
			if (remainingToggles % 2 == 0) {
				// 偶数なら点滅色
				Model_->SetDiffuseColor(flashColor_);
			}
			else {
				// 奇数なら元の色
				Model_->SetDiffuseColor(originalColor_);
			}
		}

		// 点滅終了時に確実に元色へ戻す
		if (flashTimer_ == 0) {
			Model_->SetDiffuseColor(originalColor_);
		}
	}
}
	
