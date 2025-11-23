#pragma once
#include "DirectXCommon.h"
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
class ModelCommon {
public:
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon()const { return dxCommon_; }

private:
	DirectXCommon* dxCommon_;
};