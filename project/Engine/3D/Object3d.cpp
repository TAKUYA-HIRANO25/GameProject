#include "Object3d.h"
#include "Object3dCommon.h"
#include <cassert>
#include "TextureManager.h"
#include "Model.h"
#include "ModelManager.h"

void Object3d::Initialize(ObJect3dCommon* object3dCommon)
{
	assert(object3dCommon && "object3dCommon must not be null");
	// 引数で受け取ってメンバ変数に記録する
	this->object3dCommon = object3dCommon;

	auto dx = object3dCommon->GetDxCommon();

	// 座標変換行列リソースを作る
	transformationMatrixResource = dx->CreateBufferResource(sizeof(TransformationMatrix));
	if (transformationMatrixResource) {
		HRESULT hr = transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
		assert(SUCCEEDED(hr) && transformationMatrixData && "Failed to map transformationMatrixResource");
		// 単位行列を書き込んでおく
		transformationMatrixData->WVP = MakeIdentity4x4();
		transformationMatrixData->World = MakeIdentity4x4();
	}

	// 平行光源用のリソースを作る
	directionalLightResource = dx->CreateBufferResource(sizeof(DirectionalLight));
	if (directionalLightResource) {
		HRESULT hr = directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
		assert(SUCCEEDED(hr) && directionalLightData && "Failed to map directionalLightResource");
		DirectionalLight light{ {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},1.0f };
		directionalLightData->color = light.color;
		directionalLightData->direction = light.direction;
		directionalLightData->intensity = light.intensity;
	}

	// インスタンス用マテリアルリソースを作成
	materialResource = dx->CreateBufferResource(sizeof(Material));
	if (materialResource) {
		HRESULT hr = materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
		assert(SUCCEEDED(hr) && materialData && "Failed to map materialResource");

		// デフォルトのマテリアル値
		Vector4 defaultColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		materialData->color = defaultColor;
		materialData->enableLighting = false;
		materialData->uvTransform = MakeIdentity4x4();
	}

	// Transform初期値
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	this->camera = object3dCommon->GetDefaultCamera();
}

Object3d::~Object3d() noexcept
{
	// マップされているならアンマップしてから解放
	if (transformationMatrixData && transformationMatrixResource) {
		transformationMatrixResource->Unmap(0, nullptr);
		transformationMatrixData = nullptr;
	}
	if (directionalLightData && directionalLightResource) {
		directionalLightResource->Unmap(0, nullptr);
		directionalLightData = nullptr;
	}
	if (materialData && materialResource) {
		materialResource->Unmap(0, nullptr);
		materialData = nullptr;
	}

	// ComPtrをリセット
	transformationMatrixResource.Reset();
	directionalLightResource.Reset();
	materialResource.Reset();

	model = nullptr;
	object3dCommon = nullptr;
}

void Object3d::Updata()
{
	// 3DのTransform処理
	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	if (camera) {
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	}
	else {
		worldViewProjectionMatrix = worldMatrix;
	}

	if (transformationMatrixData) {
		transformationMatrixData->WVP = worldViewProjectionMatrix;
		transformationMatrixData->World = worldMatrix;
	}
}

void Object3d::Draw()
{
	// DirectX共通とコマンドリスト取得
	auto dx = object3dCommon->GetDxCommon();
	auto cmd = dx->GetCommandList();

	// 座標変換行列CBufferの場所を設定
	if (transformationMatrixResource) {
		cmd->SetGraphicsRootConstantBufferView(kTransformRootIndex, transformationMatrixResource->GetGPUVirtualAddress());
	}

	// 平行光源CBufferの場所を設定
	if (directionalLightResource) {
		cmd->SetGraphicsRootConstantBufferView(kDirectionalLightRootIndex, directionalLightResource->GetGPUVirtualAddress());
	}

	// インスタンス毎マテリアルをルート0にセット
	if (materialResource) {
		cmd->SetGraphicsRootConstantBufferView(kMaterialRootIndex, materialResource->GetGPUVirtualAddress());
	}

	// 3Dモデルが割り当てられていれば描画
	if (model) {
		model->Draw();
	}
}

void Object3d::SetModel(const std::string& filePath)
{
	// モデルを検索してセット
	model = ModelManager::GetInstance()->FindModel(filePath);
	// model->Draw()内でSRV等を設定する想定
}

void Object3d::SetDiffuseColor(const Vector4& color)
{
	// 平行光色
	if (directionalLightData) {
		directionalLightData->color = color;
	}
	// インスタンスマテリアルの色を更新
	if (materialData) {
		materialData->color = color;
	}
}