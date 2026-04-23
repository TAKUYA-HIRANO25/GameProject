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
/// 3Dオブジェクトクラス
///
/// 概要:
/// - 単一のモデルを保持しワールド変換を管理して描画を行う
/// - 描画に必要な定数バッファを内部で確保し、毎フレーム更新。
///
/// 機能:
/// - Initialize:共通オブジェクト(ObJect3dCommon)を参照してリソースを初期化。
/// - Updata:Transformからワールド行列を作成しカメラのViewProjectionを掛け合わせてWVPを更新。
/// - Draw:モデルが設定されていれば描画コマンドを発行。
/// - SetModel:SetTranslate/SetRotate/SetScale:モデルと変換の設定。
/// - SetCamera:固有カメラを使用する場合にセット可能(未設定時はObJect3dCommonのデフォルトカメラを利用)。
///
/// 注意:
/// - ModelはModelManager経由で取得した参照を設定する想定(所有権はModelManager側)。
/// - 描画前にObJect3dCommon::SettingCommonDrawを呼んでパイプラインを設定しておく。
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

	// デストラクタ
	~Object3d() noexcept;

private:
	ObJect3dCommon* object3dCommon = nullptr;
	Model* model = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

	// インスタンス毎のマテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Material* materialData = nullptr;

	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;
	DirectionalLight* directionalLightData = nullptr;

	Transform transform;

	Camera* camera = nullptr;

	Matrix4x4 worldMatrix;
	Matrix4x4 worldViewProjectionMatrix;

	// ルートパラメータのインデックス
	static constexpr UINT kMaterialRootIndex = 0;
	static constexpr UINT kTransformRootIndex = 1;
	static constexpr UINT kDirectionalLightRootIndex = 3;
};