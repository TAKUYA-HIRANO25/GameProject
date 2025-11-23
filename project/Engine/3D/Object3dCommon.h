#pragma once
#include "DirectXCommon.h"
#include "Camera.h"
/// <summary>
/// 3Dオブジェクト共通ユーティリティ
///
/// 役割:
/// - 3Dオブジェクト描画に必要な共通パイプライン（ルートシグネチャ／PSO）を生成・保持する。
/// - DirectX 共通インタフェース（DirectXCommon）への参照を保持し、描画時の共通設定を提供する。
/// - デフォルトのカメラを保持し、Object3d がそれを利用できるようにする。
///
/// 主なメソッド:
/// - Initialize: パイプライン生成・初期化を行う。
/// - SettingCommonDraw: 描画時に共通のルートシグネチャや PSO を設定する。
/// - SetDefaultCamera / GetDefaultCamera: デフォルトカメラの設定・取得。
///
/// 注意:
/// - このクラスはレンダリングスレッド（通常はメインスレッド）でのみ操作する想定。
/// - パイプライン生成に失敗した場合はアサート等で検出する設計になっているため、呼び出し側で適切に初期化すること。
/// </summary>
class ObJect3dCommon {
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon()const { return dxCommon_; }

	void SettingCommonDraw();

	// Setter(Camera)
	void SetDefaultCamera(Camera* camera) { defaultCamera = camera; }

	// Getter(Camera)
	Camera* GetDefaultCamera() const { return defaultCamera; }

private:
	// ルートシグネチャの作成
	void createRootSignature();
	// グラフィックスパイプラインの生成
	void GeneratePipeline();

private:
	DirectXCommon* dxCommon_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	Camera* defaultCamera = nullptr;
};