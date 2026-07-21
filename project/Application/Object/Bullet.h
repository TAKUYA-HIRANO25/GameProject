#pragma once
#include <cstdint>
#include <string>
#include "FramWork/GameObject.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "MyMath.h"

/// <summary>
/// Bullet
/// - GameObject を継承し、位置/速度/寿命管理を提供
/// - 3D 表示を内包するための Initialize3D() / Draw() のデフォルト実装を追加
/// - 所有者タイプ(ownerType_) を保持し GetType() で返す
/// </summary>
class Bullet : public GameObject {
public:
	Bullet();
	virtual ~Bullet();

	// 基本初期化
	virtual void Initialize(float px, float py, float pz,
	                         float vx, float vy, float vz,
	                         int32_t lifeFrames = 60 * 5);

	// 3D 表示付き初期化（Object3d を内部で保持する）
	// modelPath: リソースパス（"Bullet/Bullet.obj" 等）
	// diffuseColor: モデルの拡散色
	// lifeFrames: 寿命（フレーム）
	// ownerType: 弾の所有者タイプ (Type::PlayerBullet / Type::EnemyBullet 等)
	void Initialize3D(ObJect3dCommon* object3dCommon,
	                  const Vector3& position, const Vector3& velocity,
	                  const std::string& modelPath = "Bullet/Bullet.obj",
	                  const Vector4& diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f },
	                  int32_t lifeFrames = 60 * 5,
	                  Type ownerType = Type::Unknown);

	// 毎フレーム更新
	void Update() override;

	// Draw: デフォルト実装を提供（model3d_ があれば描画）
	virtual void Draw() override;

	// 衝突時処理（デフォルトは消滅）
	void OnCollision() override;

	// 生死判定
	bool IsDead() const override { return isDead_; }

	// ワールド位置取得 (out-params)
	void GetWorldPosition(float& x, float& y, float& z) const override;

	// 速度取得/設定
	void GetVelocity(float& x, float& y, float& z) const;
	void SetVelocity(float vx, float vy, float vz);

	// GameObject の種別を返す（所有者タイプを返す）
	virtual Type GetType() const override { return ownerType_; }

protected:
	// 物理パラメータ
	float px_ = 0.0f;
	float py_ = 0.0f;
	float pz_ = 0.0f;
	float vx_ = 0.0f;
	float vy_ = 0.0f;
	float vz_ = 0.0f;

	int32_t lifeTimer_ = 60 * 5;
	bool isDead_ = false;

	// 3D 表示関連
	Transform modelTransform_;
	Object3d* model3d_ = nullptr;
	ObJect3dCommon* object3dCommon_ = nullptr;
	Vector3 modelPosition_{ 0.0f, 0.0f, 0.0f };
	Vector3 modelRotation_{ 0.0f, 0.0f, 0.0f };
	Vector3 modelScale_{ 1.0f, 1.0f, 1.0f };

	// 所有者タイプ
	Type ownerType_ = Type::Unknown;
};