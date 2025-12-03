#include "Particle.h"
#include "MatuilityForText.h"
Particle::Particle() {}

Particle::~Particle() {
	delete model_;
}

void Particle::Initialize(ObJect3dCommon* object3dCommon,const Vector3& position,const Vector3& velocity,int lifeFrames,const std::string& model,const Vector3& scale)
{
	object3dCommon_ = object3dCommon;
	position_ = position;
	velocity_ = velocity;
	lifeTimer_ = lifeFrames;
	scale_ = scale;

	model_ = new Object3d();
	model_->Initialize(object3dCommon_);
	model_->SetModel(model);
	model_->SetScale(scale_);
	model_->SetTranslate(position_);
}

void Particle::Update()
{
	if (isDead_) return;

	position_ += velocity_;

	// 減衰
	velocity_ *= 0.95f;

	if (--lifeTimer_ <= 0) {
		isDead_ = true;
		return;
	}

	if (model_) {
		model_->SetTranslate(position_);
		model_->Updata();
	}
}

void Particle::Draw()
{
	if (model_ && !isDead_) {
		model_->Draw();
	}
}