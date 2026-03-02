#include "Player.h"
#include "MatuilityForText.h"
#include <Xinput.h>
#include <algorithm>
#include <cmath>

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

void Player::Initialize(ObJect3dCommon* object3dCommon) {
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

	// マウス/パッド位置からレティクル位置を決定して更新
	Reticle();

	// 発射処理（スペースキーまたはコントローラ A）
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

void Player::SpriteDraw()
{
	// スプライト用 PSO 設定（背景/UI 用）
	spriteCommon_->SettingCommonDraw();
	if (isDead_ == false) {
		reticleSprite_->Draw();
	}
}

void Player::Draw()
{

	object3dCommon_->SettingCommonDraw(); // 3D描画共通設定
	if (isDead_ == false) {
		Model_->Draw();
	}

	for (PlayerBullet* bullet : bulletList_) {
		bullet->Draw();
	}
}

void Player::Move()
{
	Vector3 move = { 0,0,0 }; // 移動量
	const float kCharacterSpeed = 0.2f; // キャラクターの移動速度

	// コントローラ使用判定: 使用中ならキーボード入力を無視する
	bool controllerActive = input_->IsAnyGamepadActive();

	// --- ここでコントローラ検出に基づき入力ロックをセット ---
	if (input_) {
		if (controllerActive) {
			input_->SetKeyboardLockedByController(true);
			input_->SetMouseLockedByController(true);
		}
		else {
			// コントローラ非使用時はコントローラ由来のロックを解除する（Poose ロックは別扱い）
			input_->SetKeyboardLockedByController(false);
			input_->SetMouseLockedByController(false);
		}
	}

	// キーボード移動（コントローラ使用時は無視）
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

	// ----- パッド入力（左スティック・トリガで移動） -----
	if (input_->IsGamepadConnected(0)) {
		const float padSpeedFactor = 0.18f; // パッド感度（チューニング可）
		float lx = input_->GetLeftThumbX(0); // -1..1
		float ly = input_->GetLeftThumbY(0); // -1..1
		move.x += lx * padSpeedFactor;
		move.y += ly * padSpeedFactor;
		/*
		float lt = input_->GetLeftTrigger(0);  // 0..1
		float rt = input_->GetRightTrigger(0); // 0..1
		move.z += (rt - lt) * kCharacterSpeed * 1.5f;
		*/
	}

	// カメラの移動量を加算してワールド位置へ適用
	position_ += move + railCameraVelocity_;
}

void Player::Fire()
{
	// 長押し（ホールド）で発射。キーボードはスペース、コントローラはR2（右トリガ）を使用する。
	bool fireTriggered = false;

	// キーボードからの発射（長押しで連射）
	if (input_ && input_->PushKey(DIK_SPACE)) {
		fireTriggered = true;
	}

	// コントローラからの発射: 右トリガ（R2）を長押しで発射
	// 閾値を超えている間はホールド扱いになる
	const float kTriggerThreshold = 0.35f; // 調整可：0.0-1.0
	if (!fireTriggered && input_ && input_->IsGamepadConnected(0)) {
		float rt = input_->GetRightTrigger(0); // 0..1
		if (rt > kTriggerThreshold) {
			fireTriggered = true;
		}
	}

	// 発射処理（インターバル制御は既存ロジックを維持）
	if (fireTriggered && bulletTime <= 0) {

		bulletActive = true;
		bulletTime = 10;

		const float kBulletSpeed = 1.0f;

		// 発射元の位置と方向を決定（reticleWorldPos_ を目標にする）
		Vector3 startPos = Model_->GetTranslate();
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
		if (bulletTime > 0) {
			--bulletTime;
			if (bulletTime < 0) bulletTime = 0;
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
	if (Model_ && flashFlag == false) {
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

	// --- Reticle 側でもロック状態を設定（Move と整合） ---
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

	// クライアント座標（ピクセル）
	Vector2 cursor;

	if (controllerActive) {
		// コントローラ優先: マウスは無視。lastCursor に右スティックの移動を適用する。
		const float padReticleSpeed = 8.0f; // スティック 1.0 当たりのピクセル移動量（調整可）
		float rx = input_->GetRightThumbX(0); // -1..1
		float ry = input_->GetRightThumbY(0); // -1..1

		// 少しの入力で動かないようにする（簡易デッドゾーン）
		if (std::abs(rx) > padDead || std::abs(ry) > padDead) {
			lastCursor.x += rx * padReticleSpeed;
			lastCursor.y -= ry * padReticleSpeed; // 上下反転を補正
		}
		// 現在のカーソルとして lastCursor を使う（マウス無視）
		cursor = lastCursor;
	} else {
		// マウス優先: 実際のマウス位置で lastCursor を更新する
		Vector2 mousePos = input_->GetCursorClientPos2();
		lastCursor = mousePos;
		cursor = lastCursor;
	}

	const float width = static_cast<float>(WinApp::kClientWidth);
	const float height = static_cast<float>(WinApp::kClientHeight);

	// スプライト版レティクルのスクリーン座標更新
	// クランプ
	cursor.x = std::clamp(cursor.x, 0.0f, width - 1.0f);
	cursor.y = std::clamp(cursor.y, 0.0f, height - 1.0f);

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

// 被弾点滅ロジック。flashTimer_ と flashToggleCounter_ で赤/元色を切り替える。
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

// 追加実装: Position / Scale / Rotate のセッター / ゲッター
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
