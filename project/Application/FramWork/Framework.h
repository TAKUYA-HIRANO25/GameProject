#pragma once
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