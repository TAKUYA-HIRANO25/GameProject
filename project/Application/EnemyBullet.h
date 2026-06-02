#pragma once
#include "Bullet.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "MyMath.h"

/// <summary>
/// EnemyBullet
/// - Bulletを継承し、Object3dを使って描画する実装を提供する
/// </summary>
class EnemyBullet : public Bullet {
public:
	// コンストラクタ/デストラクタ
	EnemyBullet();
	~EnemyBullet();

	// 初期化(既存シグネチャを維持)
	void Initialize(ObJect3dCommon* object3dCommon, const Vector3& position, const Vector3& velocity);

	// 更新/描画
	void Update() override;
	void Draw() override;

	// 衝突
	void OnCollision() override;

	// out-params 版ワールド位置取得
	void GetWorldPosition(float& x, float& y, float& z) const override;

	// 既存互換（必要な場合）
	Vector3 GetWorldPosition() const;

	// GameObject タイプ
	Type GetType() const override { return Type::EnemyBullet; }

private:
	Transform modelTransform_;
	Object3d* Model_ = nullptr;
	Vector3 position_{ 0.0f, 0.0f, 0.0f };
	Vector3 rotation_{ 0.0f,0.0f,0.0f };
	Vector3 scale_{ 1.0f,1.0f,1.0f };

	static const int32_t kLifeTime = 60 * 5;

	ObJect3dCommon* object3dCommon_ = nullptr;
};

