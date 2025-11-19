#include "Player.h"
#include "MatuilityForText.h"

Player::Player()
{
}

Player::~Player()
{
	delete Model_;
	delete reticleModel_;
	bulletList_.remove_if([](PlayerBullet* bullet) {
		delete bullet;
		return true;
		});
}

void Player::Initialize(ObJect3dCommon* object3dCommon, Input* input) {
	object3dCommon_ = object3dCommon;
	Model_ = new Object3d();
	Model_->Initialize(object3dCommon);
	Model_->SetModel("box.obj");
	Model_->SetTranslate(position_);
	// レティクルモデル（小さく表示する）を作成
	reticleModel_ = new Object3d();
	reticleModel_->Initialize(object3dCommon);
	reticleModel_->SetModel("axis.obj"); // axis.obj を小さく表示してレティクルにする
	reticleModel_->SetScale({ 0.1f, 0.1f, 0.1f });
	reticleModel_->SetTranslate(position_);

	PlayerHP = 5.0f;
	input_ = new Input();
	input_ = input;
	bulletList_.remove_if([](PlayerBullet* bullet) {
		delete bullet;
		return true;
		});
}

void Player::Update()
{
	bulletList_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	Move();

	Reticle();

	Fire();

	for (PlayerBullet* bullet : bulletList_) {
		bullet->Update();
	}

	if (PlayerHP == 0) {
		isDead_ = true;
	}
	Model_->SetTranslate(position_);

	Model_->Updata();
	// reticle の更新（Object3d の行列更新）
	if (reticleModel_) {
		reticleModel_->SetTranslate(reticleWorldPos_);
		reticleModel_->Updata();
	}
}

void Player::Draw()
{

	Model_->Draw();

	// レティクルを先に描画しても問題ない（Depthテストあり）
	if (reticleModel_) {
		reticleModel_->Draw();
	}

	for (PlayerBullet* bullet : bulletList_) {
		bullet->Draw();
	}

}

void Player::Move()
{
	Vector3 move = { 0,0,0 }; //移動量
	const float kCharacterSpeed = 0.2f; // キャラクターの移動速度

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

	position_ += move;
}

void Player::Fire()
{


	if (input_->TriggerKey(DIK_SPACE) && bulletTime <= 0) {

		bulletActive = true;
		bulletTime = 10;

		const float kBulletSpeed = 1.0f;

		// レティクル位置に向かう方向を計算して速度にする
		Vector3 startPos = Model_->GetTranslate();
		Vector3 dir = { 0.0f, 0.0f, 1.0f };
		// reticleWorldPos_ が有効ならそれを使って方向を計算
		dir = { reticleWorldPos_.x - startPos.x, reticleWorldPos_.y - startPos.y, reticleWorldPos_.z - startPos.z };
		dir = Normalize(dir);
		Vector3 velocity = { dir.x * kBulletSpeed, dir.y * kBulletSpeed, dir.z * kBulletSpeed };

		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(object3dCommon_, Model_->GetTranslate(), velocity);

		bulletList_.push_back(newBullet);
	}
	else {
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
}

void Player::Reticle()
{
	// マウス位置からワールドへのレイを作り、遠方点をレティクル位置にする
	if (!input_ || !object3dCommon_) return;
	Camera* camera = object3dCommon_->GetDefaultCamera();
	if (!camera) return;

	// クライアント座標（ピクセル）
	Vector2 cursor = input_->GetCursorClientPos2();
	const float width = static_cast<float>(WinApp::kClientWidth);
	const float height = static_cast<float>(WinApp::kClientHeight);

	// NDC に変換
	float nx = (cursor.x / width) * 2.0f - 1.0f;
	float ny = -((cursor.y / height) * 2.0f - 1.0f); // 上下反転

	// 近クリップ・遠クリップでワールド座標に逆変換（逆射影）
	Vector3 ndcNear = { nx, ny, 0.0f };
	Vector3 ndcFar = { nx, ny, 1.0f };

	// 逆行列取得
	Matrix4x4 invProj = Inverse(camera->GetProjectionMatrix());
	Matrix4x4 invView = Inverse(camera->GetViewMatrix());

	// NDC -> eye -> world（TransformS を連続で使う）
	Vector3 pNear = TransformS(ndcNear, invProj); // -> eye space (then w division)
	pNear = TransformS(pNear, invView);           // -> world space

	Vector3 pFar = TransformS(ndcFar, invProj);
	pFar = TransformS(pFar, invView);

	// レイを作成
	Vector3 rayDir = { pFar.x - pNear.x, pFar.y - pNear.y, pFar.z - pNear.z };
	rayDir = Normalize(rayDir);

	// レティクル位置はレイ上の一定距離に置く（必要なら地面との交差計算に置き換える）
	Vector3 camPos = camera->GetTranslate();
	reticleWorldPos_ = { camPos.x + rayDir.x * reticleDistance_, camPos.y + rayDir.y * reticleDistance_, camPos.z + rayDir.z * reticleDistance_ };

	// reticleModel_ に設定（視認性のため少しプレイヤー側に寄せた短い距離も扱える）
	if (reticleModel_) {
		reticleModel_->SetTranslate(reticleWorldPos_);
	}
}