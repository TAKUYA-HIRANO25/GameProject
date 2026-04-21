#pragma once
#include "Camera.h"
#include "MyMath.h"
#include <random>

/// <summary>
/// RailCamera
/// 
/// 概要:
/// - カメラの経路制御(シネマティック移動・カメラ揺れ・プリスタート用オービット演出)を提供するユーティリティクラス。
/// - 内部に保持したCameraとTransformを毎フレームUpdateで更新することで、シーン側はGetCameraを通じて現在のカメラを取得して利用する。
/// 
/// 主な機能:
/// - Initialize:管理対象のCameraを設定する。
/// - StartCinematicMove/StopCinematicMove:指定Transformへ一定時間で補間して移動するシネマティック移動。
/// - StartShake/StopShake:カメラ揺れ(強さ・継続時間指定)。
/// - StartPreStartCinematic: プリスタート用の軌道(中心回転→最終Transform)演出。
/// - Update:毎フレーム呼び出して状態更新。
/// - IsBusy:演出中かどうかの判定。
/// 
/// 注意:
/// - Updateは毎フレーム呼ぶこと。演出中はIsBusyがtrueになる。
/// - マルチスレッドでの同時アクセスにはスレッド同期を行う。
/// </summary>

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

	//シネマティック/揺れ
	void StartCinematicMove(const Transform& targetTransform, float durationSeconds);
	void StopCinematicMove();
	void StartShake(float intensity, float durationSeconds);
	void StopShake();

	//プリスタート用オービット演出を開始
	// center: 回転中心
	// startRadius: 回転開始半径
	// endTransform: 演出終了時にカメラをセットするTransform
	// revolutions: 回転回数
	// durationSeconds: 演出合計時間
	void StartPreStartCinematic(const Vector3& center, float startRadius, const Transform& endTransform, float revolutions, float durationSeconds);

	//演出中か
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

	// pre-startorbit
	bool preStartActive_ = false;
	Vector3 orbitCenter_;
	float orbitStartRadius_ = 0.0f;
	Transform orbitEndTransform_;
	float orbitRevolutions_ = 0.0f;
	float orbitTimer_ = 0.0f;
	float orbitDuration_ = 0.0f;
};

