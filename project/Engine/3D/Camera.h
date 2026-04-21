#pragma once
#include "Mymath.h"

using namespace MyMath;
/// <summary>
/// カメラクラス
///
/// 概要:
/// - シーンのビュー変換（View）と射影変換（Projection）を管理するクラス。
/// - Transform（位置・回転・スケール）を保持し、Update でワールド行列／ビュー行列／射影行列／ViewProjection 行列を更新する。
///
/// 機能:
/// - SetRotate / SetTranslate / SetTransform: カメラの位置・回転を設定する。
/// - SetFovY / SetAspect / SetNearClipDistance / SetFarClipDistance: 射影パラメータを設定する。
/// - GetViewMatrix / GetProjectionMatrix / GetViewProjectionMatrix: 各行列の取得。
///
/// 注意:
/// - カメラ行列はレンダリング前に Update() を呼んで最新化する必要がある。
/// - 逆射影やスクリーンスペース変換を行う際には ViewProjection 行列とその逆行列を利用する。
/// </summary>
class Camera
{
public:
	Camera();

	// 初期化
	void Update();

	// Setter(Rotate)
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetFarClipDistance(const float& farClip) { farClipDistance = farClip; }
	void SetFovY(const float& fov) { fovY = fov; }
	void SetTransform(const Transform& transform) { this->transform = transform; }
	void SetAspect(const float& aspect) { this->aspect = aspect; }
	void SetNearClipDistance(const float& nearClip) { nearClipDistance = nearClip; }

	// Getter
	const Matrix4x4& GetWorldMatrix() const {
		Matrix4x4 world = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		return world;
	}
	const Matrix4x4& GetViewMatrix() const { return viewMatrix; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }
	Vector3& GetRotate() { return transform.rotate; }
	Vector3& GetTranslate() { return transform.translate; }
	const float& GetFarClipDistance() const { return farClipDistance; }
	const float& GetFovY() const { return fovY; }
	const Transform& GetTransform() const { return transform; }

private:
	Transform transform;
	Matrix4x4 worldMatrix;
	Matrix4x4 viewMatrix;

	Matrix4x4 projectionMatrix;
	float fovY;
	float aspect;
	float nearClipDistance;
	float farClipDistance;

	Matrix4x4 viewProjectionMatrix;
};
