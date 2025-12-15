#include "Camera.h"
#include "WinApp.h"

// Camera:
// - シーンのビュー行列・射影行列を管理するクラス。
// - transform（scale/rotate/translate）から world/view/projection を生成し、
//   Update() で最新化する設計。
// - 注意: aspect の初期化は WinApp のクライアントサイズ依存なので、ウィンドウサイズ変更時は
//         SetAspect 等で更新する必要がある。

Camera::Camera()
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
	// コンストラクタ: 初期行列を構築しておく
}

// Update:
// - transform が更新された場合に world/view/projection/viewProjection を再計算する。
// - 呼び出しタイミング: 描画前に必ず Update() を呼び、最新の行列を取得できるようにすること。
// - 注意: aspect や fovY, near/far が動的に変わる場合はそれらを先に更新してから呼ぶ。
void Camera::Update()
{
	// ワールド行列（モデル行列）を生成
	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	// プロジェクションは FOV/アスペクト/near/far に依存
	projectionMatrix = MakePerspectiveFovMatrix(fovY, aspect, nearClipDistance, farClipDistance);

	// ビュー行列はワールド行列の逆行列
	viewMatrix = Inverse(worldMatrix);

	// 最終的な ViewProjection を作成（描画時に使用）
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
}

