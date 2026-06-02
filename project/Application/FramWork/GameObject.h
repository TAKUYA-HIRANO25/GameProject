#pragma once

// Framework に依存しない汎用的な GameObject 基底クラス
class GameObject {
public:
	GameObject() = default;
	virtual ~GameObject() = default;

	// 毎フレームの更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Draw() = 0;

	// 生死判定
	virtual bool IsDead() const { return false; }

	// ワールド位置取得
	virtual void GetWorldPosition(float& x, float& y, float& z) const {
		x = 0.0f; y = 0.0f; z = 0.0f;
	}

	// 衝突時の処理
	virtual void OnCollision() {}

	// 初期化/終了
	virtual void Initialize() {}
	virtual void Finalize() {}

	// 簡易的なタイプ列挙
	enum class Type {
		Unknown,
		Player,
		Enemy,
		PlayerBullet,
		EnemyBullet,
		Effect,
	};
	// オブジェクト種別取得
	virtual Type GetType() const { return Type::Unknown; }
};