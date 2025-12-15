#include "Framework.h"

void Framework::Initialize()
{
	// デフォルト実装は何もしない
}

void Framework::Finalize()
{
	// デフォルト実装は何もしない
}

void Framework::Update()
{
	// デフォルト実装は何もしない
}

void Framework::Run()
{
	// 初期化
	Initialize();

	// メインループ
	while (!RoopOut()) {
		Update();
		// 描画は Update の中で roopOut_ を書き換える可能性があるため、
		// 常に Draw を呼ぶ（派生側で早期リターンさせることも可）
		Draw();
	}

	// 終了処理
	Finalize();
}