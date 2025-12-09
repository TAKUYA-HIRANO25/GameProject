#pragma once
#include "Object3d.h"
#include "MyMath.h"
#include "Object3dCommon.h"
#include "Input.h"
#include "WinApp.h"
#include "ModelManager.h"
#include "Model.h"
#include "ModelCommon.h"
#include "Camera.h"
#include "MatuilityForText.h"
#include "EnemyBullet.h"
#include "ParticleManager.h"

class Player;
/// <summary>
/// 敵キャラクターを表すクラス。
/// 
/// 概要:
/// - 3Dモデルを保持し、単純な移動・射撃・当たり判定を行うエンティティ。
/// - 自身で弾 (`EnemyBullet`) を生成して管理し、プレイヤーへの追尾射撃などの簡易的な攻撃を行う。
/// 
/// 主な責務:
/// - `Initialize` でモデルと初期位置を設定する。
/// - `Update` で移動・射撃タイマー・弾の更新・死亡判定を行う。
/// - `Draw` で自身と弾を描画する。
/// - `Fire` で弾を生成してリストへ追加する（発射間隔は `kFireInterval`）。
/// - `OnCollision` で被弾処理（HP 減少）を行う。
/// - `setPlayer` によりプレイヤー参照を受け取り、発射時にプレイヤー位置を参照して狙うことができる。
/// 
/// 注意事項:
/// - 本クラスはレンダリング / 更新をメインスレッドで行う前提でスレッドセーフではない。
/// - 弾はポインタで管理され、デストラクタ / 更新時に削除されるため、外部からの参照は破棄に注意すること。
/// </summary>
class Enemy
{
public:
	Enemy();
	
	~Enemy();
	// 初期化
	void Initialize(ObJect3dCommon* object3dCommon, Vector3 position);
	// 更新
	void Update();
	// 描画
	void Draw();
	//移動
	void Fire();
	//弾発射
	void FireTime();
	//移動切り替え
	void MoveTime();
	// プレイヤーのワールド位置取得
	Vector3 GetWorldPosition();
	// 当たり判定
	void OnCollision();
	// 敵の弾リスト取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }
	// 敵の生死判定
	bool IsDead() const { return isDead_; }
	// プレイヤー情報セット
	void setPlayer(Player* player) { player_ = player; }
	// パーティクルマネージャのセット
	void SetParticleManager(ParticleManager* mgr) { particleManager_ = mgr; }
	//色変え
	void ChangeColor();

	bool bulletActive = false; //弾発射フラグ

	static const int kFireInterval = 60; //弾の間隔

	static const int kMoveInterval = 360; //移動切り替え

	static const int kFlashDuration = 10; // 点滅時間（フレーム）

	// 点滅を何回繰り返すか
	static const int kFlashRepeat = 4;

private:
	// 基盤
	ObJect3dCommon* object3dCommon_ = nullptr;
	//敵の3Dモデル
	Transform modelTransform_;
	Object3d* Model_ = nullptr; // 3Dオブジェクト
	Vector3 position_; // 位置
	Vector3 rotation_; // 回転
	Vector3 scale_; // 拡大縮小
	float speed; // 移動速度
	//弾
	std::list<EnemyBullet*> bullets_;
	int Time = 0; //弾発射間隔用タイマー
	Vector3 bulletVel = { 0.0f,0.0f,0.0f };
	// 敵のHP
	float EnemyHp = 5.0f;
	//死亡フラグ
	bool isDead_ = false;
	//プレイヤー情報
	Player* player_ = nullptr;
	//移動
	Vector3 move = { 0.1f,0.0f,0.0f };
	int32_t moveTime = 0;

	//パーティクル
	ParticleManager* particleManager_ = nullptr;
	Vector3 particleVel = { 0.0f,0.0f,0.0f };
	int particleTimer_ = 0;

	// 被弾点滅用タイマー（フレーム） : 残りトグル用総フレーム数
	int flashTimer_ = 0;

	// 点滅で色を切り替える間隔カウンタ
	int flashToggleCounter_ = 0;

	// 元の色を保持して復帰するための値（初期は白）
	Vector4 originalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
};

