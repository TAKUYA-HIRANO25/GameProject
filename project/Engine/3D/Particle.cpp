#include "Particle.h"
#include "MatuilityForText.h"

Particle::Particle() {}

Particle::~Particle() {
	// 所有している Object3d を解放
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

	// 位置更新
	position_ += velocity_;

	// 減衰を適用して徐々に減速させる
	velocity_ *= 0.95f;

	// 寿命カウントダウン
	if (--lifeTimer_ <= 0) {
		isDead_ = true;
		return;
	}

	// モデルに位置を反映して行列更新
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