#include "Player.h"
#include "MatuilityForText.h"

Player::Player()
{
}

Player::~Player()
{
	delete PlayerModel;
}

void Player::Initialize(ObJect3dCommon* object3dCommon, Input* input) {
	object3dCommon_ = object3dCommon;
	PlayerModel = new Object3d();
	PlayerModel->Initialize(object3dCommon);
	PlayerModel->SetModel("axis.obj");
	PlayerModel->SetTranslate(position);

	input_ = new Input();
	input_ = input;
}

void Player::Update()
{
	Vector3 move = { 0,0,0 };

	const float kCharacterSpeed = 0.2f;

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

	if(input_->PushKey(DIK_Q)) {
		move.z += kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_E)) {
		move.z -= kCharacterSpeed;
	}
	position += move;

	PlayerModel->SetTranslate(position);

	PlayerModel->Updata();
}

void Player::Draw()
{

	PlayerModel->Draw();

}