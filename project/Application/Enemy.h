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
	
	void Initialize(ObJect3dCommon* object3dCommon, Vector3 position);
	
	void Update();
	
	void Draw();

	void Fire();

	void FireTime();

	void MoveTime();

	Vector3 GetWorldPosition();

	void OnCollision();

	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }

	bool IsDead() const { return isDead_; }

	void setPlayer(Player* player) { player_ = player; }

	bool bulletActive = false; //弾発射フラグ

	static const int kFireInterval = 60; //弾の間隔

	static const int kMoveInterval = 360; //移動切り替え
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
	// 敵のHP
	float EnemyHp = 1.0f;
	//死亡フラグ
	bool isDead_ = false;
	//プレイヤー情報
	Player* player_ = nullptr;
	//移動
	Vector3 move = { 0.1f,0.0f,0.0f };
	int32_t moveTime = 0;
};

