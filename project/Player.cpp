#include "Player.h"

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
	object3d->SetModel("box.obj");
	object3d->SetTranslate(Vector3(0.0f, 0.0f, 0.0f));

	input_ = new Input();
	input_ = input;
	
}

void Player::Update() {

}

void Player::Draw() {
	object3d->Draw();

}