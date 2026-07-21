#include "Player.h"
#include "MatuilityForText.h"
#include <Xinput.h>
#include <algorithm>
#include <cmath>
#include "GameObject.h"
#include <memory> 

Player::Player()
{
	//Initializerで初期化
}

// デストラクタ: unique_ptr により自動破棄されるため明示的 delete は不要
Player::~Player() = default;

void Player::Initialize(ObJect3dCommon* object3dCommon) {
	// 3D 共通参照を保持
	object3dCommon_ = object3dCommon;

	// プレイヤーモデル生成・初期化
	Model_ = std::make_unique<Object3d>();
	Model_->Initialize(object3dCommon);
	Model_->SetModel("Player/Player.obj");
	Model_->SetTranslate(position_);

	spriteCommon_ = SpriteCommon::GetInstance();
	if (spriteCommon_) {
		reticleSprite_ = std::make_unique<Sprite>();
		reticleSprite_->Initialize(spriteCommon_, "resources/Reticle.png");
		reticleSprite_->SetAnchorPoint({ 0.5f,0.5f });
		reticleSprite_->SetSize(Vector2(64, 64));
		// 初期位置は画面中央
		reticleSprite_->SetPosition(Vector2(float(WinApp::kClientWidth) * 0.5f, float(WinApp::kClientHeight) * 0.5f));
		// lastCursorを初期化
		lastCursor = Vector2(float(WinApp::kClientWidth) * 0.5f, float(WinApp::kClientHeight) * 0.5f);
	}

	// 初期色・ステータスを保存
	originalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	isDead_ = false;
	PlayerHP = 10.0f;

	// 入力はシングルトンから取得
	input_ = Input::GetInstance();

	// bulletList_を念のためクリア
	bulletList_.clear();
}

void Player::Update()
{
	// 死亡した弾の解放
	bulletList_.remove_if([](const std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});
	// 被弾時の点滅処理
	ChangeColor();

	// 入力に基づく移動処理
	Move();

	// マウス/パッド位置からレティクル位置を決定して更新
	Reticle();

	// 発射処理
	Fire();

	// 所有弾の更新
	for (const auto& bullet : bulletList_) {
		bullet->Update();
	}

	// HPが0なら死亡フラグを立てる
	if (PlayerHP == 0) {
		isDead_ = true;
	}

	// モデルに位置を反映して行列更新
	if (Model_) {
		Model_->SetTranslate(position_);
		Model_->Updata();
	}
}

void Player::SpriteDraw()
{
	// スプライト用PSO設定(背景/UI)
	spriteCommon_->SettingCommonDraw();
	if (isDead_ == false && reticleSprite_) {
		reticleSprite_->Draw();
	}
}

void Player::Draw()
{
	object3dCommon_->SettingCommonDraw(); // 3D描画共通設定
	if (isDead_ == false && Model_) {
		Model_->Draw();
	}

	for (const auto& bullet : bulletList_) {
		bullet->Draw();
	}
}

void Player::Move()
{
	Vector3 move = { 0,0,0 }; // 移動量
	const float kCharacterSpeed = 0.2f; // キャラクターの移動速度

	// コントローラ使用判定:使用中ならキーボード入力を無視する
	bool controllerActive = input_->IsAnyGamepadActive();

	// ここでコントローラ検出に基づき入力ロックをセット
	if (input_) {
		if (controllerActive) {
			input_->SetKeyboardLockedByController(true);
			input_->SetMouseLockedByController(true);
		}
		else {
			// コントローラ非使用時はコントローラ由来のロックを解除
			input_->SetKeyboardLockedByController(false);
			input_->SetMouseLockedByController(false);
		}
	}

	// キーボード移動
	if (!controllerActive) {
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
	}

	// パッド入力（左スティック・トリガで移動)
	if (input_->IsGamepadConnected(0)) {
		const float padSpeedFactor = 0.18f; // パッド感度
		float lx = input_->GetLeftThumbX(0); // -1..1
		float ly = input_->GetLeftThumbY(0); // -1..1
		move.x += lx * padSpeedFactor;
		move.y += ly * padSpeedFactor;
	}

	// カメラの移動量を加算してワールド位置へ適用
	position_ += move + railCameraVelocity_;
}

void Player::Fire()
{
	// 長押しで発射。キーボードはスペース、コントローラはR2を使用する。
	bool fireTriggered = false;

	// キーボードからの発射
	if (input_ && input_->PushKey(DIK_SPACE)) {
		fireTriggered = true;
	}

	// コントローラからの発射:R2を長押しで発射
	const float kTriggerThreshold = 0.35f; // 調整可：0.0-1.0
	if (!fireTriggered && input_ && input_->IsGamepadConnected(0)) {
		float rt = input_->GetRightTrigger(0); // 0..1
		if (rt > kTriggerThreshold) {
			fireTriggered = true;
		}
	}

	// 発射処理
	if (fireTriggered && bulletTime <= 0) {

		bulletActive = true;
		bulletTime = 10;

		const float kBulletSpeed = 1.0f;

		// 発射元の位置と方向を決定
		Vector3 startPos = Model_->GetTranslate();
		dir = { reticleWorldPos_.x - startPos.x, reticleWorldPos_.y - startPos.y, reticleWorldPos_.z - startPos.z };
		dir = Normalize(dir);
		Vector3 velocity = { dir.x * kBulletSpeed, dir.y * kBulletSpeed, dir.z * kBulletSpeed };
		velocity += railCameraVelocity_; // カメラ移動を弾速に加味

		// 弾オブジェクト生成・初期化
		auto newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(object3dCommon_, Model_->GetTranslate(), velocity);
		bulletList_.push_back(std::move(newBullet));
	}
	else {
		// 発射インターバルのカウントダウン
		if (bulletTime > 0) {
			--bulletTime;
			if (bulletTime < 0) bulletTime = 0;
		}
	}
}

void Player::GetWorldPosition(float& x, float& y, float& z) const
{
	x = position_.x;
	y = position_.y;
	z = position_.z;
}

void Player::OnCollision()
{
	PlayerHP -= 1;

	// パーティクルの生成(被弾エフェクト)
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

	// 被弾点滅を開始
	if (Model_ && flashFlag == false) {
		flashTimer_ = flashDuration_ * flashRepeat_ * 2;
		flashToggleCounter_ = flashDuration_;
		Model_->SetDiffuseColor(flashColor_);
	}
}

std::vector<PlayerBullet*> Player::GetBulletsRaw() const
{
	return std::vector<PlayerBullet*>();
}

void Player::Reticle()
{
	// 必要な参照が無ければ早期リターン
	if (!input_ || !object3dCommon_) return;
	Camera* camera = object3dCommon_->GetDefaultCamera();
	if (!camera) return;

	// コントローラ使用判定
	const float padDead = 0.15f;

	if (input_->IsGamepadConnected(0)) {
		float lx = input_->GetLeftThumbX(0);
		float ly = input_->GetLeftThumbY(0);
		float rx = input_->GetRightThumbX(0);
		float ry = input_->GetRightThumbY(0);
		float lt = input_->GetLeftTrigger(0);
		float rt = input_->GetRightTrigger(0);

		if (std::abs(lx) > padDead || std::abs(ly) > padDead ||
			std::abs(rx) > padDead || std::abs(ry) > padDead ||
			lt > 0.05f || rt > 0.05f) {
			controllerActive = true;
		}
		if (!controllerActive) {
			if (input_->GamepadButtonPush(0, XINPUT_GAMEPAD_A) ||
				input_->GamepadButtonPush(0, XINPUT_GAMEPAD_B) ||
				input_->GamepadButtonPush(0, XINPUT_GAMEPAD_X) ||
				input_->GamepadButtonPush(0, XINPUT_GAMEPAD_Y) ||
				input_->GamepadButtonPush(0, XINPUT_GAMEPAD_DPAD_UP) ||
				input_->GamepadButtonPush(0, XINPUT_GAMEPAD_DPAD_DOWN) ||
				input_->GamepadButtonPush(0, XINPUT_GAMEPAD_DPAD_LEFT) ||
				input_->GamepadButtonPush(0, XINPUT_GAMEPAD_DPAD_RIGHT)) {
				controllerActive = true;
			}
		}
	}

	// Reticle側でもロック状態を設定
	if (input_) {
		if (controllerActive) {
			input_->SetKeyboardLockedByController(true);
			input_->SetMouseLockedByController(true);
		}
		else {
			input_->SetKeyboardLockedByController(false);
			input_->SetMouseLockedByController(false);
		}
	}

	// クライアント座標
	Vector2 cursor;

	if (controllerActive) {
		// コントローラ優先:マウスは無視lastCursorに右スティックの移動を適用する。
		const float padReticleSpeed = 8.0f; // スティック1.0当たりのピクセル移動量
		float rx = input_->GetRightThumbX(0); // -1..1
		float ry = input_->GetRightThumbY(0); // -1..1

		// 少しの入力で動かないようにする（簡易デッドゾーン）
		if (std::abs(rx) > padDead || std::abs(ry) > padDead) {
			lastCursor.x += rx * padReticleSpeed;
			lastCursor.y -= ry * padReticleSpeed; // 上下反転を補正
		}
		// 現在のカーソルとしてlastCursorを使う
		cursor = lastCursor;
	}
	else {
		// マウス優先:ロック中なら相対移動を使用、そうでなければ絶対位置を取得してlastCursorを更新する
		bool mouseLocked = input_->IsMouseLockedByPoose() || input_->IsMouseLockedByController();

		if (mouseLocked) {
			// 相対移動量を取得してlastCursorに加算する

			int mx = input_->GetMouseMoveX();
			int my = input_->GetMouseMoveY();
			lastCursor.x += static_cast<float>(mx);
			lastCursor.y += static_cast<float>(my);
			cursor = lastCursor;
		}
		else {
			// 通常はクライアント座標を直接参照してlastCursorを更新
			Vector2 mousePos = input_->GetCursorClientPos2();
			lastCursor = mousePos;
			cursor = lastCursor;
		}
	}

	const float width = static_cast<float>(WinApp::kClientWidth);
	const float height = static_cast<float>(WinApp::kClientHeight);

	// スプライト版レティクルのスクリーン座標更新
	// クランプ
	cursor.x = std::clamp(cursor.x, 0.0f, width - 1.0f);
	cursor.y = std::clamp(cursor.y, 0.0f, height - 1.0f);

	if (reticleSprite_) {
		reticleSprite_->SetPosition(cursor);
		reticleSprite_->Update();
	}
	// NDCに変換
	float nx = (cursor.x / width) * 2.0f - 1.0f;
	float ny = -((cursor.y / height) * 2.0f - 1.0f); // 上下反転

	// 近クリップ、遠クリップのNDCを定義
	Vector3 ndcNear = { nx, ny, 0.0f };
	Vector3 ndcFar = { nx, ny, 1.0f };

	// カメラの逆行列を取得してNDC->ワールド変換
	Matrix4x4 invProj = Inverse(camera->GetProjectionMatrix());
	Matrix4x4 invView = Inverse(camera->GetViewMatrix());

	Vector3 pNear = TransformS(ndcNear, invProj);
	pNear = TransformS(pNear, invView);

	Vector3 pFar = TransformS(ndcFar, invProj);
	pFar = TransformS(pFar, invView);

	// レイを作成して正規化
	Vector3 rayDir = { pFar.x - pNear.x, pFar.y - pNear.y, pFar.z - pNear.z };
	rayDir = Normalize(rayDir);

	// レティクル位置はカメラ位置 + rayDir * 距離
	Vector3 camPos = camera->GetTranslate();
	reticleWorldPos_ = { camPos.x + rayDir.x * reticleDistance_, camPos.y + rayDir.y * reticleDistance_, camPos.z + rayDir.z * reticleDistance_ };

	// reticleModel_にセット
	if (reticleModel_) {
		reticleModel_->SetTranslate(reticleWorldPos_);
	}
}

// レールカメラの移動速度を受け取り、プレイヤーの動きに反映するために保持。
void Player::SetRailCameraVelocity(Vector3 velocity)
{
	railCameraVelocity_ = velocity;
}

// 被弾点滅ロジック。flashTimer_とflashToggleCounter_で赤/元色を切り替える。
void Player::ChangeColor()
{
	if (flashTimer_ > 0 && Model_) {
		flashFlag = true;
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
			flashFlag = false;
		}
	}
}

// Position/Scale/Rotateのセッター/ゲッター
void Player::SetPosition(const Vector3& pos)
{
	position_ = pos;
	if (Model_) {
		Model_->SetTranslate(position_);
		Model_->Updata();
	}
}

void Player::SetScale(const Vector3& s)
{
	scale = s;
	if (Model_) {
		Model_->SetScale(scale);
		Model_->Updata();
	}
}

void Player::SetRotate(const Vector3& r)
{
	rotation = r;
	if (Model_) {
		Model_->SetRotate(rotation);
		Model_->Updata();
	}
}

Vector3 Player::GetScale() const
{
	if (Model_) {
		return Model_->GetScale();
	}
	return scale;
}

Vector3 Player::GetRotate() const
{
	if (Model_) {
		return Model_->GetRotate();
	}
	return rotation;
}