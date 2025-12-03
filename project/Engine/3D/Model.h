#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <chrono>
#include <vector>
#include <fstream>
#include <sstream>
#include "MyMath.h"

/// <summary>
/// 3Dモデル
///
/// 概要:
/// - .obj ファイルなどから頂点／マテリアルを読み込み、GPU バッファを作成して描画を行うクラス。
/// - ModelCommon 経由で DirectX の共通機能にアクセスする。
///
/// 主なメソッド:
/// - Initialize: ファイル読み込みとバッファ作成を行う。
/// - Draw: 作成済みバッファを使って描画コマンドを発行する。
/// - static LoadObjFile / LoadMaterialTemplateFile: ファイル読み込みユーティリティ（静的）。
///
/// 注意:
/// - Initialize で GPU リソースを確保するため、Finalize 相当の処理（管理側での解放）が必要。
/// - 大きなモデルの読み込みは I/O と GPU リソース確保のコストがあるため起動時や非同期化を検討すること。
/// </summary>
// 3Dモデル
using namespace MyMath;

class ModelCommon;

// 頂点データ構造体
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};
// マテリアル構造体
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};
// マテリアルデータ構造体
struct MaterialData {
	std::string textureFilePath;
	uint32_t textureIndex = 0;
};

// モデルデータ構造体
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

// 3Dモデル
class Model {
public:
	// 初期化
	void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename);
	// 描画
	void Draw();
	// 頂点バッファビュー取得
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	// OBJファイル読み込み
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

private:
	ModelCommon* modelCommon = nullptr;

	ModelData modelData;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;

	VertexData* vertexData = nullptr;
	Material* materialData = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

};