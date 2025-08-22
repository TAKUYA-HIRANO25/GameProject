#include "Player.h"
#include "Engine/Math/MatuilityForText.h"

Player::Player()
{
}

Player::~Player()
{
	delete object3d;	
}

void Player::Initialize(ObJect3dCommon* object3dCommon,Input* input) {
	object3d = new Object3d();
	object3d->Initialize(object3dCommon);
	object3d->SetModel("Box.obj");
	object3d->SetTranslate(position);

	input_ = new Input();
	input_ = input;
	
}

void Player::Update() {
	Vector3 move = { 0,0,0 };

	const float kCharacterSpeed = 0.2f;

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

	position += move;

	object3d->SetTranslate(position);
}

void Player::Draw() {
	object3d->Draw();

}