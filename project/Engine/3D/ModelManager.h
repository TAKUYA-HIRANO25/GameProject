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
/// - モデル読み込み/描画で共通して必要になるDirectXCommonなどの参照を保持する軽量クラス。
/// - Modelクラス群がレンダリングやリソース作成で利用する共通機能を提供する場所として機能。
///
/// 使用:
/// - ModelManagerやModelのInitialize時にDirectXCommonを渡して初期化。
/// - GetDxCommonでDirectXCommonへのアクセスを行う。
/// 
/// 注意:
/// - ModelManagerがModelCommonのライフサイクルを管理するため、ModelCommonの使用はModelManagerやModelクラス内に限定すること。
/// </summary>
// モデルマネージャー
class ModelManager {
private:
	static ModelManager* instance;
	// コンストラクタ・デストラクタ
	ModelManager() = default;
	~ModelManager() = default;
	// コピーコンストラクタ・代入演算子削除
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager& other) = delete;

public:
	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	// 終了
	void Finalize();

	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	//モデルファイルの読み
	void LoadModel(const std::string& filePath);

	/// モデルの
	Model* FindModel(const std::string& filePath);

private:
	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models;

	ModelCommon* modelCommon = nullptr;
};