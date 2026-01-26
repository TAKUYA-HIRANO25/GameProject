#include "RailCamera.h"
#include <cmath>
#include <chrono>
#include <cstdio>
#include <Windows.h>

// 線形補間
static inline float LerpF(float a, float b, float t) noexcept { return a + (b - a) * t; }
static inline Vector3 LerpV(const Vector3& a, const Vector3& b, float t) noexcept {
	return { LerpF(a.x, b.x, t), LerpF(a.y, b.y, t), LerpF(a.z, b.z, t) };
}
static inline Transform LerpT(const Transform& a, const Transform& b, float t) noexcept {
	return { LerpV(a.scale, b.scale, t), LerpV(a.rotate, b.rotate, t), LerpV(a.translate, b.translate, t) };
}

RailCamera::RailCamera()
	: camera_(nullptr),
	  rng_(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())),
	  dist_(-1.0f, 1.0f)
{
	velocity_ = { 0.0f,0.0f,0.0f };
	transform_ = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
}

void RailCamera::Initialize(Camera* camera)
{
	camera_ = camera;
	if (camera_) {
		transform_ = camera_->GetTransform();
	}
}

void RailCamera::StartCinematicMove(const Transform& targetTransform, float durationSeconds)
{
	if (durationSeconds <= 0.0f) {
		transform_ = targetTransform;
		cinematicActive_ = false;
		if (camera_) { camera_->SetTransform(transform_); camera_->Update(); }
		return;
	}
	cinematicActive_ = true;
	cinematicStartTransform_ = transform_;
	cinematicTargetTransform_ = targetTransform;
	cinematicTimer_ = 0.0f;
	cinematicDuration_ = durationSeconds;
}

void RailCamera::StopCinematicMove()
{
	cinematicActive_ = false;
	cinematicTimer_ = 0.0f;
	cinematicDuration_ = 0.0f;
}

void RailCamera::StartShake(float intensity, float durationSeconds)
{
	if (intensity <= 0.0f || durationSeconds <= 0.0f) return;
	shakeActive_ = true;
	shakeIntensity_ = intensity;
	shakeDuration_ = durationSeconds;
	shakeTimer_ = 0.0f;
}

void RailCamera::StopShake()
{
	shakeActive_ = false;
	shakeTimer_ = 0.0f;
	shakeDuration_ = 0.0f;
	shakeIntensity_ = 0.0f;
}

void RailCamera::StartPreStartCinematic(const Vector3& center, float startRadius, const Transform& endTransform, float revolutions, float durationSeconds)
{
	// デバッグ出力：演出開始
	{
		char buf[256];
		sprintf_s(buf, "RailCamera::StartPreStartCinematic: center=(%f,%f,%f) startRadius=%f revolutions=%f duration=%f end=(%f,%f,%f)\n",
			center.x, center.y, center.z, startRadius, revolutions, durationSeconds,
			endTransform.translate.x, endTransform.translate.y, endTransform.translate.z);
		OutputDebugStringA(buf);
	}

	if (durationSeconds <= 0.0f) {
		transform_ = endTransform;
		if (camera_) { camera_->SetTransform(transform_); camera_->Update(); }
		preStartActive_ = false;
		return;
	}
	preStartActive_ = true;
	orbitCenter_ = center;
	orbitStartRadius_ = startRadius;
	orbitEndTransform_ = endTransform;
	orbitRevolutions_ = revolutions;
	orbitTimer_ = 0.0f;
	orbitDuration_ = durationSeconds;
}

bool RailCamera::IsBusy() const
{
	return cinematicActive_ || preStartActive_ || shakeActive_;
}

void RailCamera::Update()
{
	// 固定デルタ（簡易）
	constexpr float kDelta = 1.0f / 60.0f;

	// cinematic
	if (cinematicActive_) {
		cinematicTimer_ += kDelta;
		float t = cinematicDuration_ > 0.0f ? (cinematicTimer_ / cinematicDuration_) : 1.0f;
		if (t >= 1.0f) {
			transform_ = cinematicTargetTransform_;
			cinematicActive_ = false;
		}
		else {
			float s = t * t * (3.0f - 2.0f * t);
			transform_ = LerpT(cinematicStartTransform_, cinematicTargetTransform_, s);
		}
	}
	else if (preStartActive_) {
		orbitTimer_ += kDelta;
		float t = orbitDuration_ > 0.0f ? (orbitTimer_ / orbitDuration_) : 1.0f;
		if (t >= 1.0f) {
			// 終了時は endTransform を厳密にセット
			transform_ = orbitEndTransform_;
			preStartActive_ = false;

			// デバッグ：終了ログ
			char buf[256];
			sprintf_s(buf, "RailCamera::PreStart END: final pos=(%f,%f,%f)\n",
				transform_.translate.x, transform_.translate.y, transform_.translate.z);
			OutputDebugStringA(buf);
		}
		else {
			// 半径を線形に endRadius へ近づける（endRadius = distance(center, endTransform.translate)）
			auto dx = orbitEndTransform_.translate.x - orbitCenter_.x;
			auto dy = orbitEndTransform_.translate.y - orbitCenter_.y;
			auto dz = orbitEndTransform_.translate.z - orbitCenter_.z;
			float endRadius = std::sqrt(dx*dx + dy*dy + dz*dz);
			float radius = LerpF(orbitStartRadius_, endRadius, t);

			// 角度 = 2PI * revolutions * eased_t
			float eased = t * t * (3.0f - 2.0f * t);
			float angle = 2.0f * 3.14159265358979323846f * orbitRevolutions_ * eased;

			// Y を lerp して上下を演出
			float y = LerpF(orbitCenter_.y + 0.5f, orbitEndTransform_.translate.y, eased);

			// カメラ位置（X,Z 平面で円運動）
			Vector3 pos;
			pos.x = orbitCenter_.x + radius * std::cos(angle);
			pos.y = y;
			pos.z = orbitCenter_.z + radius * std::sin(angle);

			// カメラが常に中心を見るように向きを計算（LookAt の簡易計算）
			Vector3 dir;
			dir.x = orbitCenter_.x - pos.x;
			dir.y = orbitCenter_.y - pos.y;
			dir.z = orbitCenter_.z - pos.z;

			// yaw（Y軸回転）
			float yaw = std::atan2(dir.x, dir.z); // radians

			// pitch（X軸回転）
			float horizDist = std::sqrt(dir.x * dir.x + dir.z * dir.z);
			float pitch = std::atan2(dir.y, horizDist); // radians

			// Apply to transform: position と回転を設定
			Transform temp = orbitEndTransform_;
			temp.translate = pos;
			temp.rotate.x = pitch; // 上下向き
			temp.rotate.y = yaw;   // 水平向き（ラジアン）
			transform_ = temp;

			// デバッグ出力：毎フレームの位置と角度（軽めに）
			{
				char buf[256];
				sprintf_s(buf, "RailCamera::PreStart t=%0.3f pos=(%0.3f,%0.3f,%0.3f) yaw=%0.3f pitch=%0.3f radius=%0.3f\n",
					t, pos.x, pos.y, pos.z, yaw, pitch, radius);
				OutputDebugStringA(buf);
			}
		}
	}
	else {
		// 通常速度適用
		transform_.translate.x += velocity_.x;
		transform_.translate.y += velocity_.y;
		transform_.translate.z += velocity_.z;
	}

	// 揺れ
	Vector3 shakeOffset{ 0.0f,0.0f,0.0f };
	if (shakeActive_) {
		shakeTimer_ += kDelta;
		float life = (shakeDuration_ > 0.0f) ? (shakeTimer_ / shakeDuration_) : 1.0f;
		if (life >= 1.0f) {
			shakeActive_ = false;
		}
		float currentIntensity = shakeIntensity_ * (1.0f - life);
		shakeOffset.x = dist_(rng_) * currentIntensity;
		shakeOffset.y = dist_(rng_) * currentIntensity;
		shakeOffset.z = dist_(rng_) * currentIntensity * 0.2f;
	}

	Transform applied = transform_;
	applied.translate.x += shakeOffset.x;
	applied.translate.y += shakeOffset.y;
	applied.translate.z += shakeOffset.z;

	if (camera_) {
		camera_->SetTransform(applied);
		camera_->Update();
	}
}
