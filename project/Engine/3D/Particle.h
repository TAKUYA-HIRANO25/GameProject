#pragma once
#include "Object3d.h"
#include "MyMath.h"
#include "Object3dCommon.h"
#include <string>

using namespace MyMath;

/// <summary>
/// Particle
///
/// 概要:
/// - 単一のパーティクルを表す軽量クラス。位置・速度・寿命を持ち、内部でObject3dを利用して描画。
/// 
/// 主な機能:
/// - Initialize:ObJect3dCommonと初期位置・速度・寿命・モデルを指定して初期化。
/// - Update:位置更新と寿命管理を行い、寿命が尽きれば内部フラグを立てる。
/// - Draw:内部のObject3dを使って描画。
/// - IsDead:寿命切れかどうかを問い合わせる。
/// 
/// 注意:
/// - Object3dの所有権は本クラスが持つ想定(Initializeで生成/取得する実装に依存)。外部で管理する場合は注意すること。
/// - デフォルトのモデルファイルはParticle.obj、デフォルトスケールは小さめに設定されている。
/// </summary>

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