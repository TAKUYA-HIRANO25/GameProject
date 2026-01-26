#pragma once
#include "Camera.h"
#include "MyMath.h"
#include <random>
	
class RailCamera
{
public:
	RailCamera();
	//初期化
	void Initialize(Camera* camera);
	//更新
	void Update();
	//カメラ取得
	Camera* GetCamera() { return camera_; }
	//変換情報取得
	Transform& GetTransform() { return  transform_; }
	Vector3 GetVelocity() { return velocity_; }

	// シネマティック / 揺れ API
	void StartCinematicMove(const Transform& targetTransform, float durationSeconds);
	void StopCinematicMove();
	void StartShake(float intensity, float durationSeconds);
	void StopShake();

	// --- プリスタート用オービット演出を開始 ---
	// center: 回転中心 (通常はプレイヤー位置)
	// startRadius: 回転開始半径
	// endTransform: 演出終了時にカメラをセットする Transform (例 {0,0,-20})
	// revolutions: 回転回数
	// durationSeconds: 演出合計時間（秒）
	void StartPreStartCinematic(const Vector3& center, float startRadius, const Transform& endTransform, float revolutions, float durationSeconds);

	// 演出中か（Cinematic/PreStart/Shake のいずれか）
	bool IsBusy() const;

private:
	Transform transform_;
	Vector3 velocity_;
	Camera* camera_;

	// cinematic
	bool cinematicActive_ = false;
	Transform cinematicStartTransform_;
	Transform cinematicTargetTransform_;
	float cinematicTimer_ = 0.0f;
	float cinematicDuration_ = 0.0f;

	// shake
	bool shakeActive_ = false;
	float shakeIntensity_ = 0.0f;
	float shakeTimer_ = 0.0f;
	float shakeDuration_ = 0.0f;
	std::mt19937 rng_;
	std::uniform_real_distribution<float> dist_;

	// pre-start orbit
	bool preStartActive_ = false;
	Vector3 orbitCenter_;
	float orbitStartRadius_ = 0.0f;
	Transform orbitEndTransform_;
	float orbitRevolutions_ = 0.0f;
	float orbitTimer_ = 0.0f;
	float orbitDuration_ = 0.0f;
};

