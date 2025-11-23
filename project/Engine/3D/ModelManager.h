#pragma once

#include "DirectXCommon.h"
#include <wrl.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <map>

class Model;

class ModelCommon;

/// <summary>
/// 3Dモデル共通部
///
/// 概要:
/// - モデル読み込み／描画で共通して必要になる DirectXCommon などの参照を保持する軽量クラス。
/// - Model クラス群がレンダリングやリソース作成で利用する共通機能を提供する場所として機能する。
///
/// 使用:
/// - ModelManager や Model の Initialize 時に DirectXCommon を渡して初期化する。
/// - GetDxCommon() で DirectXCommon へのアクセスを行う。
/// </summary>
// モデルマネージャー
class ModelManager {
private:
	static ModelManager* instance;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager& other) = delete;

public:
	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	// 終了
	void Finalize();

	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	///  <summary>{
	///  モデルファイルの読み込み
	/// </summary>
	//<param name="filepath">モデルファイルのパス</param>
	void LoadModel(const std::string& filePath);

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath">モデルファイルのパス</param>
	/// <return>モデル</return>
	Model* FindModel(const std::string& filePath);

private:
	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models;

	ModelCommon* modelCommon = nullptr;
};