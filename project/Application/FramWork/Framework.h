#pragma once

/// <summary>
/// Framework
///
/// 概要:
/// - アプリケーションの基本ループ(初期化・更新・描画・終了)を提供する抽象基底クラス。
/// - 派生クラスはInitialize/Update/FinalizeをオーバーライドしDrawを必ず実装して描画処理を行う。
/// - Runによってメインループが実行される想定(派生クラスの Update/Draw を毎フレーム呼ぶ)。
/// 
/// 主な機能:
/// - Initialize/Finalize/Update:ライフサイクル管理用の仮想メソッド。
/// - Draw:純粋仮想。
/// - Run:実行ループの開始。内部でRoopOutによる終了判定を利用する。
/// - RoopOut:ループ継続判定(派生クラスで制御可能)。
/// 
/// 注意:
/// - 派生クラスはroopOutを操作してループ終了を制御できる。
/// </summary>

class Framework
{
public:
	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 毎フレーム更新
	virtual void Update();

	// 描画
	virtual void Draw() = 0;

	// 実行
	void Run();

	// 終了チェック
	virtual bool RoopOut() { return roopOut_; }

	virtual ~Framework() = default;

protected:
	// 派生クラスがループ停止を制御できるよう protected に移動
	bool roopOut_ = false;
};