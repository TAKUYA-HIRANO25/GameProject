#pragma once
#include "Particle.h"
#include "Vector3.h"

class ParticleManager {
public:
	ParticleManager(ObJect3dCommon* object3dCommon);
	~ParticleManager();

	void Spawn(const Vector3& position,
		const Vector3& velocity,
		int lifeFrames = 30,
		const std::string& modelFile = "box.obj",
		const Vector3& scale = { 0.12f,0.12f,0.12f });

	void Update();
	void Draw();

private:
	ObJect3dCommon* object3dCommon_ = nullptr;
	std::vector<Particle*> particles_;
};