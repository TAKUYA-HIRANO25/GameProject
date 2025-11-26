#pragma once
#include "Object3d.h"
#include "MyMath.h"
#include "Object3dCommon.h"
#include <string>

using namespace MyMath;

class Particle {
public:
	Particle();
	~Particle();

	void Initialize(ObJect3dCommon* object3dCommon,
		const Vector3& position,
		const Vector3& velocity,
		int lifeFrames = 30,
		const std::string& modelFile = "Particle.obj",
		const Vector3& scale = { 0.12f,0.12f,0.12f });

	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }

	Vector3 GetWorldPosition() const { return position_; }

private:
	ObJect3dCommon* object3dCommon_ = nullptr;
	Object3d* model_ = nullptr;
	Vector3 position_{ 0,0,0 };
	Vector3 velocity_{ 0,0,0 };
	Vector3 scale_{ 1,1,1 };
	int lifeTimer_ = 0;
	bool isDead_ = false;
};