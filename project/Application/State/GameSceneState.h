#pragma once
#include <memory>

class GameScene;

// State の抽象インターフェイス（GameScene 用）
class GameSceneState {
public:
	virtual ~GameSceneState() = default;
	// 状態に入ったときに呼ばれる
	virtual void Enter(GameScene* scene) {}
	// 毎フレームの更新
	virtual void Update(GameScene* scene) = 0;
	// 描画タイミングの処理（必要ならオーバーライド）
	virtual void Draw(GameScene* scene) {}
	// 状態を抜けるときに呼ばれる
	virtual void Exit(GameScene* scene) {}
};

// ファクトリ（必要な State を追加していく）
std::unique_ptr<GameSceneState> CreateReadyState();
std::unique_ptr<GameSceneState> CreateGoState();
std::unique_ptr<GameSceneState> CreateMainState();
std::unique_ptr<GameSceneState> CreateGameOverState();
std::unique_ptr<GameSceneState> CreateClearState();