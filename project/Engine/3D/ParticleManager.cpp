#include "ParticleManager.h"
#include <algorithm>

ParticleManager::ParticleManager(ObJect3dCommon* object3dCommon){
	// 3D 共通ユーティリティ参照を保持
	object3dCommon_ = object3dCommon;
}

ParticleManager::~ParticleManager()
{
	// 全てのパーティクルを解放
	for (Particle* p : particles_) {
		delete p;
	}
	particles_.clear();
}

// 新しいパーティクルを生成してリストに追加。
void ParticleManager::Spawn(const Vector3& position,const Vector3& velocity,int lifeFrames,const std::string& modelFile,const Vector3& scale)
{
	Particle* p = new Particle();
	p->Initialize(object3dCommon_, position, velocity, lifeFrames, modelFile, scale);
	particles_.push_back(p);
}

// 全パーティクルを Update し、寿命切れのものを削除。
void ParticleManager::Update()
{
	for (Particle* p : particles_) {
		p->Update();
	}

	particles_.erase(std::remove_if(particles_.begin(), particles_.end(),
		[](Particle* p) {
			if (p->IsDead()) { delete p; return true; }
			return false;
		}), particles_.end());
}

// 生存している全パーティクルを描画。
void ParticleManager::Draw()
{
	for (Particle* p : particles_) {
		p->Draw();
	}
}
