#pragma once
#include "DirectXCommon.h"

/// <summary>
/// スプライト描画の共通ユーティリティクラス
///
/// 役割:
/// - スプライト描画に必要な DirectX 12 のルートシグネチャと PSO（パイプラインステート）を生成・保持する。
/// - 描画時に共通設定（ルートシグネチャ/PSO/プリミティブトポロジなど）を行うラッパーを提供する。
/// - Sprite クラス群が描画で必要とする DirectXCommon 参照を保持する。
///
/// 主な機能:
/// - Initialize(DirectXCommon*): DirectXCommon を受け取り、ルートシグネチャとパイプラインを初期化する。
/// - SettingCommonDraw(): 描画開始時に共通のルートシグネチャと PSO をコマンドリストに設定する。
/// - getDxCommon(): 内部で保持している DirectXCommon へのアクセスを提供する。
///
/// 注意:
/// - 初期化はアプリケーションの起動時に一度だけ行うこと（スレッドセーフではない）。
/// - パイプライン生成に失敗した場合はアサート等で検出する実装になっている可能性があるため、呼び出し側で適切に扱うこと。
/// </summary>

class SpriteCommon {
public:
	// シングルトン取得（Meyers）
	static SpriteCommon* GetInstance();

	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	// 描画共通設定
	void SettingCommonDraw();

	// DirectXCommon取得
	DirectXCommon* getDxCommon()const { return dxCommon_; }

private:
	// プライベート化（シングルトン）
	SpriteCommon() = default;
	~SpriteCommon() = default;
	SpriteCommon(const SpriteCommon&) = delete;
	SpriteCommon& operator=(const SpriteCommon&) = delete;
	SpriteCommon(SpriteCommon&&) = delete;
	SpriteCommon& operator=(SpriteCommon&&) = delete;

	void RootSignatureInitialize();

	void GeneratePipelineInitialize();

	DirectXCommon* dxCommon_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	static SpriteCommon* instance;
};
