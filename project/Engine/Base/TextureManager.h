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
/// - テクスチャファイルを読み込みGPUへアップロードしてSRVを作成。
/// - ファイルパスからテクスチャインデックスを取得できるマップ機能を提供。
/// - テクスチャのメタデータを提供。
/// 
/// 使用:
/// -  アプリ起動時にInitializeを呼びDirectXCommonを渡す。
/// -  LoadTextureでテクスチャを登録。
/// -  描画時にGetSrvHandleGPUを参照。
/// 
/// 備考:
/// - シングルトンなのでグローバルにアクセス可能Finalizeで後片付けを行うこと。
/// - スレッドセーフではないため、起動シーケンスやリソース登録はメインスレッドで行うことを推奨。
/// </summary>

// テクスチャ管理クラス
class TextureManager {
public:

	// シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	// 終了
	void Finalize();

	// 初期化
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