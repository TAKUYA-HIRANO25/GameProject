#include "Player.h"
#include "MatuilityForText.h"

Player::Player()
{
}

Player::~Player()
{
	delete Model_;
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

	Fire();

	for (PlayerBullet* bullet : bulletList_) {
		bullet->Update();
	}

	if (PlayerHP == 0) {
		isDead_ = true;
	}
	Model_->SetTranslate(position_);

	Model_->Updata();
}

void Player::Draw()
{

	Model_->Draw();

	for (PlayerBullet* bullet : bulletList_) {
		bullet->Draw();
	}

}

void Player::Move()
{
	Vector3 move = { 0,0,0 }; //移動量
	const float kCharacterSpeed = 0.2f; // キャラクターの移動速度

	if (input_->PushKey(DIK_A)) {
		move.x += kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_D)) {
		move.x -= kCharacterSpeed;
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

		const float kBulletSpeed = -1.0f;

		Vector3 velocity(0, 0, kBulletSpeed);

		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(object3dCommon_,Model_->GetTranslate(), velocity);

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
