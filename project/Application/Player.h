#pragma once
#include <memory>
#include <list>
#include "Object3d.h"
#include "MyMath.h"
#include "Object3dCommon.h"
#include "Input.h"
#include "WinApp.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "ModelManager.h"
#include "Model.h"
#include "ModelCommon.h"
#include "Camera.h"
#include "MatuilityForText.h"
#include "Bullet.h"
#include "ParticleManager.h"
#include "FramWork/GameObject.h"
/// <summary>
/// プレイヤーを表すクラス
///
/// 概要:
/// - 3Dモデルを保持し、移動、射撃、被弾処理、レティクルを管理するエンティティ。
/// - 自身でPlayerBulletを生成してリストで管理し、弾を制御。
///
/// 主な機能:
/// - Initialize:モデルや入力参照の初期設定。
/// - Update:移動:レティクル更新・射撃処理・弾の更新・死亡判定。
/// - Draw:プレイヤー本体と弾、レティクルの描画を行う。
/// - Move/Fire:ユーザー入力に基づく移動と弾発射処理。
/// - Reticle:マウス位置からワールド空間の照準位置を算出してレティクルを更新。
///
/// 注意事項:
/// - 弾やレティクルは生ポインタで管理されるため、外部参照がある場合は破棄タイミングに注意。
/// - レンダリングと更新はメインスレッドで行う前提でスレッドセーフではない。
/// - マウス→ワールド変換の実装はカメラ行列や座標系に依存するため、必要に応じてNDC・Y軸反転等を調整。
/// </summary>
class Player : public GameObject
{
public:
	Player();
	~Player();
	void Initialize(ObJect3dCommon* object3dCommon);
	void Update() override;
	void SpriteDraw();
	void Draw() override;
	void Move();
	void Fire();
	void GetWorldPosition(float& x, float& y, float& z) const override;
	void OnCollision() override;
	// 弾リストは unique_ptr<Bullet>
	const std::list<std::unique_ptr<Bullet>>& GetBullets() const { return bulletList_; }
	std::vector<Bullet*> GetBulletsRaw() const;
	void Reticle();
	void SetParticleManager(ParticleManager* mgr) { particleManager_ = mgr; }
	bool IsDead() const override { return isDead_; }
	void SetRailCameraVelocity(Vector3 velocity);
	bool bulletActive = false;

	// フラッシュ等...
	void SetFlashColor(const Vector4& color) { flashColor_ = color; }
	void SetFlashDuration(int frames) { flashDuration_ = frames; }
	void SetFlashRepeat(int repeat) { flashRepeat_ = repeat; }

	void SetPosition(const Vector3& pos);
	void SetScale(const Vector3& s);
	void SetRotate(const Vector3& r);
	Vector3 GetScale() const;
	Vector3 GetRotate() const;
	Vector3 GetPosition() const { return position_; }

private:
	ObJect3dCommon* object3dCommon_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;

	std::unique_ptr<Object3d> Model_{};
	Vector3 position_ = { 0.0f, -4.0f, 10.0f };
	Vector3 translation;
	Vector3 rotation;
	Vector3 scale;
	float speed;
	Input* input_ = nullptr;

	// 弾リストもunique_ptrで管理（Bullet に統合）
	std::list<std::unique_ptr<Bullet>> bulletList_;
	int bulletTime = 0;
	int bulletFlag = 0;
	Vector3 dir = { 0.0f, 0.0f, 1.0f };

	std::unique_ptr<Object3d> reticleModel_{};
	Vector3 reticleWorldPos_ = { 0.0f, 0.0f, 0.0f };
	float reticleDistance_ = 100.0f;
	std::unique_ptr<Sprite> reticleSprite_{};
	Vector2 lastCursor;
	bool controllerActive = false;

	float PlayerHP = 10.0f;
	bool isDead_ = false;

	ParticleManager* particleManager_ = nullptr;
	int particleTimer_ = 0;

	Vector3 railCameraVelocity_ = { 0.0f, 0.0f, 0.0f };

	static const int kDefaultFlashDuration = 10;
	static const int kDefaultFlashRepeat = 4;
	int flashTimer_ = 0;
	int flashToggleCounter_ = 0;
	int flashDuration_ = kDefaultFlashDuration;
	int flashRepeat_ = kDefaultFlashRepeat;
	Vector4 flashColor_ = { 1.0f, 0.25f, 0.25f, 1.0f };
	Vector4 originalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	bool flashFlag = false;
	void ChangeColor();
};