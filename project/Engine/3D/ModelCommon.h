#pragma once
#include "DirectXCommon.h"
/// <summary>
/// 3Dモデル共通部
/// </summary>

class ModelCommon {
public:
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon()const { return dxCommon_; }

private:
	DirectXCommon* dxCommon_;
};