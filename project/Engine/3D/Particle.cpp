#include "Particle.h"
#include "MatuilityForText.h"

// Particle:
// - 単一のパーティクル表現（単純なモデルを使ったエフェクト）。
// - 所有する Object3d を内部で管理し、寿命・位置・速度・スケールを保持する。
// - Initialize でモデルを生成し、Update で物理的移動と寿命を管理、Draw で描画する。
// - 注意: model_ は new で確保するためデストラクタで delete する。

Particle::Particle() {}

Particle::~Particle() {
	// 所有している Object3d を解放
	delete model_;
}

// Initialize:
// - object3dCommon を保持し、モデルを生成して初期 transform を設定する。
// - lifeFrames はパーティクルの寿命（フレーム数）。
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

// Update:
// - 毎フレーム位置を velocity で更新し、速度に減衰をかける。
// - lifeTimer_ が 0 以下になれば isDead_ を立てる。
// - model_ が存在すればその transform を反映して行列更新（Updata）を呼ぶ。
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

// Draw:
// - モデルが存在かつ生存している場合に描画を行う。
// - 描画前の PSO 設定やカメラ設定は呼び出し元が行う想定。
void Particle::Draw()
{
	if (model_ && !isDead_) {
		model_->Draw();
	}
}