#include "ParticleManager.h"
#include <algorithm>

// ParticleManager:
// - Particle オブジェクトの生成・更新・描画・破棄を一括管理する。
// - 所有権: Spawn で new した Particle はこのクラスが所有し、寿命切れで delete する。
// - スレッド: レンダリング / 更新はメインスレッド（レンダリングスレッド）で呼ぶ想定。
// - 性能: 多数のパーティクル発生時はコンテナ操作と new/delete が頻発するため、必要ならプールを導入する。

ParticleManager::ParticleManager(ObJect3dCommon* object3dCommon){
	// 3D 共通ユーティリティ参照を保持（描画時に使用）
	object3dCommon_ = object3dCommon;
}

ParticleManager::~ParticleManager()
{
	// 全てのパーティクルを解放
	for (Particle* p : particles_) {
		delete p;
	}
	particles_.clear();
}

// Spawn:
// - 新しいパーティクルを生成してリストに追加する。
// - 引数: 位置、初速、寿命フレーム数、表示用モデルファイル名、スケール
void ParticleManager::Spawn(const Vector3& position,const Vector3& velocity,int lifeFrames,const std::string& modelFile,const Vector3& scale)
{
	Particle* p = new Particle();
	p->Initialize(object3dCommon_, position, velocity, lifeFrames, modelFile, scale);
	particles_.push_back(p);
}

// Update:
// - 全パーティクルを Update し、寿命切れのものを削除する。
// - 削除は remove_if + delete のパターンで安全に行う。
// - 注意: delete 後に参照が残らないよう、外部ではポインタを保持しないこと。
void ParticleManager::Update()
{
	for (Particle* p : particles_) {
		p->Update();
	}

	particles_.erase(std::remove_if(particles_.begin(), particles_.end(),
		[](Particle* p) {
			if (p->IsDead()) { delete p; return true; }
			return false;
		}), particles_.end());
}

// Draw:
// - 生存している全パーティクルを描画する。
// - 描画順序や深度テストは呼び出し元（Object3dCommon / MyGame::Draw）が管理すること。
void ParticleManager::Draw()
{
	for (Particle* p : particles_) {
		p->Draw();
	}
}
