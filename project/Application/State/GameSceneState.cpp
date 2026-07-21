#include "GameSceneState.h"
#include "GameScene.h"
#include <cstdio>

// ReadyState
struct ReadyStateImpl : public GameSceneState {
	void Enter(GameScene* scene) override {
		std::puts("[State] Enter Ready");
		// 初期タイマーリセット等が必要ならここで
	}
	void Update(GameScene* scene) override {
		// 既存の ready ロジックを GameScene に移譲
		scene->UpdateReady();
	}
	void Draw(GameScene* scene) override {
		scene->DrawReady();
	}
};

// GoState
struct GoStateImpl : public GameSceneState {
	void Enter(GameScene* scene) override {
		std::puts("[State] Enter Go");
	}
	void Update(GameScene* scene) override {
		scene->UpdateGo();
	}
	void Draw(GameScene* scene) override {
		scene->DrawGo();
	}
};

// MainState
struct MainStateImpl : public GameSceneState {
	void Enter(GameScene* scene) override {
		std::puts("[State] Enter Main");
	}
	void Update(GameScene* scene) override {
		// 実ゲームロジックは GameScene 側に移譲
		scene->UpdateMain();
	}
	void Draw(GameScene* scene) override {
		scene->DrawMain();
	}
};

// GameOverState
struct GameOverStateImpl : public GameSceneState {
	void Enter(GameScene* scene) override {
		std::puts("[State] Enter GameOver");
	}
	void Update(GameScene* scene) override {
		scene->UpdateGameOver();
	}
	void Draw(GameScene* scene) override {
		scene->DrawGameOver();
	}
};

// ClearState
struct ClearStateImpl : public GameSceneState {
	void Enter(GameScene* scene) override {
		std::puts("[State] Enter Clear");
	}
	void Update(GameScene* scene) override {
		scene->UpdateClear();
	}
	void Draw(GameScene* scene) override {
		scene->DrawClear();
	}
};

// ファクトリ実装
std::unique_ptr<GameSceneState> CreateReadyState() { return std::make_unique<ReadyStateImpl>(); }
std::unique_ptr<GameSceneState> CreateGoState() { return std::make_unique<GoStateImpl>(); }
std::unique_ptr<GameSceneState> CreateMainState() { return std::make_unique<MainStateImpl>(); }
std::unique_ptr<GameSceneState> CreateGameOverState() { return std::make_unique<GameOverStateImpl>(); }
std::unique_ptr<GameSceneState> CreateClearState() { return std::make_unique<ClearStateImpl>(); }