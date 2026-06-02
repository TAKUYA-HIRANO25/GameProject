#pragma once
#include <cstdint>
#include "FramWork/GameObject.h"

/// <summary>
/// Bullet
/// - GameObject を継承することでシーンで一元管理可能にする
/// - 位置/速度/寿命管理の基本実装を提供し、描画や衝突処理は派生クラスで実装する想定
/// </summary>
class Bullet : public GameObject {
public:
	Bullet() = default;
	virtual ~Bullet() = default;

	// 初期化: フレームワーク型は使わず基本パラメータのみ受け取る
	// position:px,py,pz
	// velocity:vx,vy,vz
	// lifeFrames:寿命（フレーム）
	virtual void Initialize(float px, float py, float pz,
	                         float vx, float vy, float vz,
	                         int32_t lifeFrames = 60 * 5);

	// 毎フレーム更新
	void Update() override;

	// 描画は派生で必須実装
	virtual void Draw() override = 0;

	// 衝突時処理（デフォルトは消滅）
	void OnCollision() override;

	// 生死判定
	bool IsDead() const override { return isDead_; }

	// ワールド位置取得 (out-params)
	void GetWorldPosition(float& x, float& y, float& z) const override;

	// 速度取得/設定（必要なら利用）
	void GetVelocity(float& x, float& y, float& z) const;
	void SetVelocity(float vx, float vy, float vz);

	// GameObject の種別を返すデフォルト実装
	virtual Type GetType() const override { return Type::Unknown; }

protected:
	float px_ = 0.0f;
	float py_ = 0.0f;
	float pz_ = 0.0f;
	float vx_ = 0.0f;
	float vy_ = 0.0f;
	float vz_ = 0.0f;

	int32_t lifeTimer_ = 60 * 5;
	bool isDead_ = false;
};