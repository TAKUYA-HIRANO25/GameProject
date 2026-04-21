#pragma once
#include "DirectXCommon.h"
#include "Camera.h"
/// <summary>
/// 3Dオブジェクト共通ユーティリティ
///
/// 役割:
/// - 3Dオブジェクト描画に必要な共通パイプラインを生成・保持する。
/// - DirectX共通インタフェース(DirectXCommon)への参照を保持し描画時の共通設定を提供する。
/// - デフォルトのカメラを保持し、Object3d がそれを利用できるようにする。
///
/// 機能:
/// - Initialize:パイプライン生成・初期化を行う。
/// - SettingCommonDraw:描画時に共通のルートシグネチャやPSOを設定する。
/// - SetDefaultCamera/GetDefaultCamera: デフォルトカメラの設定・取得。
///
/// 注意:
/// - このクラスはレンダリングスレッドでのみ操作する想定。
/// - パイプライン生成に失敗した場合はアサート等で検出する設計になっているため、呼び出し側で適切に初期化すること。
/// </summary>

// 3Dオブジェクト共通クラス
class ObJect3dCommon {
public:
	// シングルトン取得
	static ObJect3dCommon* GetInstance();

	// 初期化
	void Initialize(DirectXCommon* dxCommon);
	// DirectXCommon取得
	DirectXCommon* GetDxCommon()const { return dxCommon_; }
	// 描画共通設定
	void SettingCommonDraw();
	// Setter
	void SetDefaultCamera(Camera* camera) { defaultCamera = camera; }
	// Getter
	Camera* GetDefaultCamera() const { return defaultCamera; }

private:
	// シングルトン
	ObJect3dCommon() = default;
	~ObJect3dCommon() = default;
	ObJect3dCommon(const ObJect3dCommon&) = delete;
	ObJect3dCommon& operator=(const ObJect3dCommon&) = delete;
	ObJect3dCommon(ObJect3dCommon&&) = delete;
	ObJect3dCommon& operator=(ObJect3dCommon&&) = delete;

	// ルートシグネチャの作成
	void createRootSignature();
	// グラフィックスパイプラインの生成
	void GeneratePipeline();

private:
	DirectXCommon* dxCommon_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	Camera* defaultCamera = nullptr;

	static ObJect3dCommon* instance;
};