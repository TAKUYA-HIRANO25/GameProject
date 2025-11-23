#pragma once
#pragma once
#include <fstream>
#include <d3d12.h>
#include <wrl.h>
#include <chrono>
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex//d3dx12.h"

class DirectXCommon;

/// <summary>
/// テクスチャ管理シングルトン.
/// 
/// 機能:
/// - テクスチャファイルを読み込み、GPU へアップロードして SRV を作成する。
/// - ファイルパスからテクスチャインデックスを取得できるマップ機能を提供する。
/// - テクスチャのメタデータ（幅・高さなど）を提供する。
/// 
/// 使用手順:
/// 1. アプリ起動時に __Initialize__ を呼び、DirectXCommon を渡す。
/// 2. __LoadTexture(filePath)__ でテクスチャを登録する（複数回呼び出し可）。
/// 3. 描画時に __GetSrvHandleGPU(index)__ や __GetMetaData(index)__ を参照する。
/// 
/// 備考:
/// - シングルトンなのでグローバルにアクセス可能。Finalize で後片付けを行うこと。
/// - スレッドセーフではないため、起動シーケンスやリソース登録はメインスレッドで行うことを推奨。
/// </summary>
class TextureManager {
public:

	// シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	// 終了
	void Finalize();

	void Initialize(DirectXCommon* dxCommon);

	// テクスチャファイルのパス
	void LoadTexture(const std::string& filePath);

	// SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	// テスクチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureInex);

	// メタデータを取得
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

private:
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

	// テスクチャ1枚分のデータ
	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	// テスクチャデータ
	std::vector<TextureData> textureDatas;

	DirectXCommon* dxCommon_ = nullptr;

	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

};