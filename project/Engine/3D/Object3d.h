#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <chrono>
#include <vector>
#include <fstream>
#include <sstream>
#include "Mymath.h"
#include "Camera.h"
using namespace MyMath;

class ObJect3dCommon;
class Model;

/// <summary>
/// 3Dオブジェクトを表すクラス
///
/// 概要:
/// - 単一のモデル（`Model`）を保持し、ワールド変換（Transform）を管理して描画を行う軽量ラッパー。
/// - 描画に必要な定数バッファ（WVP / World）を内部で確保し、毎フレーム更新する。
///
/// 主な機能:
/// - Initialize: 共通オブジェクト（ObJect3dCommon）を参照してリソースを初期化する。
/// - Updata: Transform からワールド行列を作成し、カメラの ViewProjection を掛け合わせて WVP を更新する。
/// - Draw: モデルが設定されていれば描画コマンドを発行する。
/// - SetModel / SetTranslate / SetRotate / SetScale: モデルと変換の設定。
/// - SetCamera: 固有カメラを使用する場合にセット可能（未設定時は ObJect3dCommon のデフォルトカメラを利用）。
///
/// 注意:
/// - `Model` は ModelManager 経由で取得した参照を設定する想定（所有権は ModelManager 側）。
/// - 描画前に ObJect3dCommon::SettingCommonDraw を呼んでパイプラインを設定しておくこと。
/// </summary>
// 3Dオブジェクト
class Object3d {
public:

public:
	// 初期化
	void Initialize(ObJect3dCommon* object3dCommon);
	// 更新
	void Updata();
	// 描画
	void Draw();

	// setter
	void SetModel(const std::string& filePath);
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetCamera(Camera* camera) { this->camera = camera; }
	void SetDiffuseColor(const Vector4& color);

	// Getter
	const Vector3& GetScale()const { return transform.scale; }
	const Vector3& GetRotate()const { return transform.rotate; }
	const Vector3& GetTranslate()const { return transform.translate; }

private:
	ObJect3dCommon* object3dCommon = nullptr;
	Model* model = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

	// インスタンス毎のマテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Material* materialData = nullptr;

	// バッファリソース内のデータをさすポインタ
	TransformationMatrix* transformationMatrixData = nullptr;
	DirectionalLight* directionalLightData = nullptr;

	Transform transform;

	Camera* camera = nullptr;

	Matrix4x4 worldMatrix;
	Matrix4x4 worldViewProjectionMatrix;
};