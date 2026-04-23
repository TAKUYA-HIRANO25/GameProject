#pragma once
#include "Particle.h"
#include <vector>
#include <string>

/// <summary>
/// ParticleManager
/// 
/// 概要:
/// - Particleオブジェクトの生成・更新・描画・破棄を一括管理するクラス。
/// - SpawnでnewしたParticleはこのクラスが所有し、寿命切れでdelete。
/// - Updateで全Particleを更新し、寿命切れのものを削除。
/// - Drawで全Particleを描画。
/// - IsEmptyで現在パーティクルが存在しないかを問い合わせることができる。
///	
/// 主な機能:
/// - Initialize:ObJect3dCommonの参照を保持するコンストラクタと、全Particleを解放するデストラクタ。
/// - Spawn:新しいParticleを生成してリストに追加。
/// - Update:全ParticleをUpdateし、寿命切れのものを削除。
/// - Draw:生存している全 Particleを描画。
/// - IsEmpty:現在パーティクルが存在しないかを問い合わせる。
/// 
/// 注意:
/// - レンダリング/更新はメインスレッド(レンダリングスレッド)で呼ぶ想定でスレッドセーフではない。
/// - 多数のパーティクル発生時はコンテナ操作と new/delete が頻発するため、必要ならプールを導入すること。
/// </summary>


class ParticleManager {
public:
	ParticleManager(ObJect3dCommon* object3dCommon);
	~ParticleManager();

	void Spawn(const Vector3& position,
		const Vector3& velocity,
		int lifeFrames = 30,
		const std::string& modelFile = "box.obj",
		const Vector3& scale = { 0.12f,0.12f,0.12f });

	void Update();
	void Draw();

	// 現在パーティクルが存在しないかを問い合わせる（爆発エフェクトの終了判定用）
	bool IsEmpty() const { return particles_.empty(); }

private:
	ObJect3dCommon* object3dCommon_ = nullptr;
	std::vector<Particle*> particles_;
};