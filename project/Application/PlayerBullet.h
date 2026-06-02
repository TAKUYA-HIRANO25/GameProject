#pragma once
#include "Bullet.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "MyMath.h"

/// <summary>
/// PlayerBullet
/// - Bullet を継承し、Object3d を使って描画する実装を提供する
/// - out-params 版 GetWorldPosition を override し、既存コード互換のため Vector3 返却版をラップで残す
/// </summary>
class PlayerBullet : public Bullet {
public:
	// コンストラクタ / デストラクタ
	PlayerBullet();
	~PlayerBullet();

	// 初期化（既存シグネチャを維持）
	void Initialize(ObJect3dCommon* object3dCommon, const Vector3& position, const Vector3& velocity);

	// 更新 / 描画
	void Update() override;
	void Draw() override;

	// 衝突
	void OnCollision() override;

	// GameObject インターフェース準拠: out-params 版
	void GetWorldPosition(float& x, float& y, float& z) const override;

	// 互換用: Vector3 返却版（既存コードが利用する箇所があればこちらを使用）
	Vector3 GetWorldPosition() const;

	// 生死判定は基底を利用
	bool IsDead() const { return Bullet::IsDead(); }

	// GameObject タイプ
	Type GetType() const override { return Type::PlayerBullet; }

private:
	Transform modelTransform_;
	Object3d* Model_ = nullptr;
	Vector3 position_{ 0.0f, 0.0f, 0.0f };
	Vector3 rotation_{ 0.0f,0.0f,0.0f };
	Vector3 scale_{ 1.0f,1.0f,1.0f };

	static const int32_t kLifeTime = 60 * 5;

	ObJect3dCommon* object3dCommon_ = nullptr;
};

