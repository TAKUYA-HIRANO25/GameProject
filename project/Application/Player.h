#pragma once
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
#include "PlayerBullet.h"
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
	// 初期化
	void Initialize(ObJect3dCommon* object3dCommon);
	// 更新
	void Update() override;
	//スプライト描画
	void SpriteDraw();
	// 描画
	void Draw() override;
	//移動
	void Move();
	//弾発射
	void Fire();
	// プレイヤーのワールド位置取得（out-params 版）
	void GetWorldPosition(float& x, float& y, float& z) const override;
	// 当たり判定
	void OnCollision() override;
	// プレイヤーの弾リスト取得
	const std::list<PlayerBullet*>& GetBullets() const { return bulletList_; }
	// レティクル更新
	void Reticle();
	// パーティクルマネージャセット
	void SetParticleManager(ParticleManager* mgr) { particleManager_ = mgr; }
	// プレイヤーの生死判定
	bool IsDead() const override { return isDead_; }
	//レールカメラ用にプレイヤーのTransformをセット
	void SetRailCameraVelocity(Vector3 velocity);
	bool bulletActive = false; //弾発射フラグ

	// 点滅設定API
	void SetFlashColor(const Vector4& color) { flashColor_ = color; }
	void SetFlashDuration(int frames) { flashDuration_ = frames; }
	void SetFlashRepeat(int repeat) { flashRepeat_ = repeat; }

	// Position/Scale/Rotateの外部操作と取得
	void SetPosition(const Vector3& pos);
	void SetScale(const Vector3& s);
	void SetRotate(const Vector3& r);
	Vector3 GetScale() const;
	Vector3 GetRotate() const;

private:
	// 基盤
	ObJect3dCommon* object3dCommon_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;

	//プレイヤー
	Object3d* Model_ = nullptr; // 3Dオブジェクト
	Vector3 position_ = { 0.0f, -4.0f, 10.0f }; // 位置
	Vector3 translation; // 位置
	Vector3 rotation; // 回転
	Vector3 scale; // 拡大縮小
	float speed; // 移動速度
	// 入力関連
	Input* input_;
	//弾関連
	std::list<PlayerBullet*> bulletList_;
	int bulletTime = 0; //弾発射間隔用タイマー
	int bulletFlag = 0;
	Vector3 dir = { 0.0f, 0.0f, 1.0f };

	// レティクル関連
	Object3d* reticleModel_;    // 画面上の3Dレティクル表示用
	Vector3 reticleWorldPos_ = { 0.0f, 0.0f, 0.0f }; // レティクルのワールド位置
	float reticleDistance_ = 100.0f; // レティクルまでの距離
	Sprite* reticleSprite_ = nullptr;
	Vector2 lastCursor; // 前フレームのカーソル位置保持用
	bool controllerActive = false; // コントローラ使用中フラグ

	float PlayerHP = 10.0f;
	bool isDead_ = false;

	//パーティクル
	ParticleManager* particleManager_ = nullptr; 
	int particleTimer_ = 0;

	//レールカメラ速度
	Vector3 railCameraVelocity_ = { 0.0f, 0.0f, 0.0f };

	// 被弾点滅関連
	static const int kDefaultFlashDuration = 10;
	static const int kDefaultFlashRepeat = 4;
	int flashTimer_ = 0;               // 残りフレーム数
	int flashToggleCounter_ = 0;       // トグル間隔カウンタ
	int flashDuration_ = kDefaultFlashDuration;
	int flashRepeat_ = kDefaultFlashRepeat;
	Vector4 flashColor_ = { 1.0f, 0.25f, 0.25f, 1.0f }; // デフォルト赤
	Vector4 originalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f }; // 元の色保持

	bool flashFlag = false;
	// 点滅処理ヘルパ
	void ChangeColor();

};