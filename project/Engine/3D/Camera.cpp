#include "Camera.h"
#include "WinApp.h"

Camera::Camera()
//初期行列を構築しておく
	: transform({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} })
	, fovY(0.45f)
	, aspect(float(WinApp::kClientWidth / float(WinApp::kClientHeight)))
	, nearClipDistance(0.1f)
	, farClipDistance(100.0f)
	, worldMatrix(MakeAffineMatrix(transform.scale, transform.rotate, transform.translate))
	, viewMatrix(Inverse(worldMatrix))
	, projectionMatrix(MakePerspectiveFovMatrix(fovY, aspect, nearClipDistance, farClipDistance))
	, viewProjectionMatrix(Multiply(viewMatrix, projectionMatrix))
{

}

void Camera::Update()
{
	// ワールド行列（モデル行列)を生成
	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	// プロジェクションはFOV/アスペクト/near/farに依存
	projectionMatrix = MakePerspectiveFovMatrix(fovY, aspect, nearClipDistance, farClipDistance);

	// ビュー行列はワールド行列の逆行列
	viewMatrix = Inverse(worldMatrix);

	// 最終的なViewProjectionを作成
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
}

