#pragma once
#include "DirectXCommon.h"
/// <summary>
/// 3Dモデル共通部
///
/// 概要:
/// - モデル読み込み/描画で共通してDirectXCommonなどの参照を保持する軽量クラス。
/// - Modelクラス群がレンダリングやリソース作成で利用する共通機能を提供する場所として機能する。
///
/// 使用:
/// - ModelManagerやModelのInitialize時にDirectXCommonを渡して初期化。
/// - GetDxCommonでDirectXCommonへのアクセスを行う。
/// 
/// 注意:
/// - ModelCommonはDirectXCommonの参照を保持するだけの軽量クラスで、リソース管理や描画ロジックはModelやModelManagerに任せる。
/// 
/// </summary>

class ModelCommon {
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);
	// DirectXCommon 取得
	DirectXCommon* GetDxCommon()const { return dxCommon_; }

private:
	DirectXCommon* dxCommon_;
};