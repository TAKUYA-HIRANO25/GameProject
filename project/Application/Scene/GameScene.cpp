#include "GameScene.h"
#include "GameSceneState.h"
#include <windows.h>
#include <cstdio>
#include <memory>

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	// unique_ptr が自動で破棄するため何もしない
}

void GameScene::Initialize(RailCamera* railCamera)
{
	winApp_ = WinApp::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	spriteCommon_ = SpriteCommon::GetInstance();
	object3dCommon_ = ObJect3dCommon::GetInstance();

	// 所有オブジェクトを make_unique で生成
	particleManager_ = std::make_unique<ParticleManager>(object3dCommon_);

	railCamera_ = railCamera;

	player_ = std::make_unique<Player>();
	player_->Initialize(object3dCommon_);
	player_->SetRailCameraVelocity(railCamera_->GetVelocity());
	player_->SetParticleManager(particleManager_.get());

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(object3dCommon_, Vector3{ 0.0f, -1.0f, 60.0f });
	enemy_->setPlayer(player_.get());
	enemy_->SetParticleManager(particleManager_.get());

	objects_.clear();
	objects_.push_back(static_cast<GameObject*>(player_.get()));
	objects_.push_back(static_cast<GameObject*>(enemy_.get()));

	skyDome_ = std::make_unique<Object3d>();
	skyDome_->Initialize(object3dCommon_);
	skyDome_->SetModel("sphere.obj");
	skyDome_->SetScale({ 50.0f, 50.0f, 50.0f });
	skyDome_->SetTranslate({ 0.0f, -1.0f, 20.0f });

	Ready_ = std::make_unique<Sprite>();
	Ready_->Initialize(spriteCommon_, "resources/Ready.png");
	Go_ = std::make_unique<Sprite>();
	Go_->Initialize(spriteCommon_, "resources/GO.png");
	gameOver_ = std::make_unique<Sprite>();
	gameOver_->Initialize(spriteCommon_, "resources/GameOver.png");
	clear_ = std::make_unique<Sprite>();
	clear_->Initialize(spriteCommon_, "resources/GameClear.png");
	EndUI_ = std::make_unique<Sprite>();
	EndUI_->Initialize(spriteCommon_, "resources/EndUI.png");
	Black_ = std::make_unique<Sprite>();
	Black_->Initialize(spriteCommon_, "resources/backGround.png"); Black_->SetSize(Vector2(1280, 720)); Black_->SetColor(Vector4(1, 1, 1, 0));
	explanation_ = std::make_unique<Sprite>();
	explanation_->Initialize(spriteCommon_, "resources/Explanation.png");

	poose_ = std::make_unique<Poose>();
	poose_->Initialize();

	isGame_ = true;
	isSet_ = false;
	startTime_ = 0;
	goTime_ = 0;

	currentScene_ = Scene::ready;

	state_ = CreateReadyState();
	if (state_) state_->Enter(this);

	cursorHidden_ = false;
	if (!poose_->IsActive()) {
		ShowCursor(FALSE);
		cursorHidden_ = true;
	}
}

void GameScene::Update()
{
	// pendingStateがセットされていればここで適用
	if (pendingState_) {
		if (state_) {
			state_->Exit(this);
			state_ = std::move(pendingState_);
		}
		if (state_) {
			state_->Enter(this);
		}
	}

	if (state_) {
		state_->Update(this);
		return;
	}

	if (skyDome_) skyDome_->Updata();

#ifdef USE_IMGUI
	ImGui::ShowDemoWindow();
#endif
}

void GameScene::Draw()
{
	// pendingStateを反映
	if (pendingState_) {
		if (state_) {
			state_->Exit(this);
		 state_ = std::move(pendingState_);
		}
		if (state_) { 
			state_->Enter(this); 
		}
	}

	// State があれば描画を委譲
	if (state_) {
		state_->Draw(this);
		return;
	}

	Camera* cam = nullptr;
	if (object3dCommon_) cam = object3dCommon_->GetDefaultCamera();

	object3dCommon_->SettingCommonDraw();

	// フォールバック描画
	switch (currentScene_) {
		case GameScene::Scene::ready:
			DrawReady();
			break;
		case GameScene::Scene::Go:
			DrawGo();
			break;
		case GameScene::Scene::main:
			DrawMain();
			break;
		case GameScene::Scene::gameOver:
			DrawGameOver();
			break;
		case GameScene::Scene::clear:
			DrawClear();
			break;
		default:
			break;
	}
}

// 切り出した各シーン処理

void GameScene::UpdateReady()
{
	++startTime_;
	Ready_->Update();
	if (startTime_ >= 220) {
		// State へ移行
		RequestStateChange(CreateGoState());
	}
}

void GameScene::UpdateGo()
{
	++goTime_;
	Go_->Update();
	if (goTime_ >= 120) {
		RequestStateChange(CreateMainState());
	}
}

void GameScene::UpdateMain()
{
	if (input_->TriggerKey(DIK_ESCAPE) || input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_START)) {
		if (!poose_->IsActive()) {
			poose_->Activate();
		}
		else {
			poose_->Deactivate();
		}
	}
	// ポーズが有効ならPooseを先に更新し
	if (poose_->IsActive()) {
		// 表示状態が変わった場合のみShowCursorを呼ぶ
		if (cursorHidden_) {
			ShowCursor(TRUE);
			cursorHidden_ = false;
		}

		poose_->Update();

		auto r = poose_->GetResult();
		if (r != Poose::Result::None) {
			if (r == Poose::Result::Resume) {
				poose_->Deactivate();
			}
			else if (r == Poose::Result::ToTitle) {
				poose_->Deactivate();
				RequestStateChange(CreateReadyState());
				isGame_ = false;
				isSet_ = true;
			}
			poose_->ClearResult();
		}

		// ポーズ中はゲームロジック更新をスキップしてフェード処理へ
		return;
	}
	else {
		// ポーズ解除時は表示を隠す
		if (!cursorHidden_) {
			ShowCursor(FALSE);
			cursorHidden_ = true;
		}
	}
	// パーティクル更新
	if (particleManager_) particleManager_->Update();

	// GameObject一括Update
	for (GameObject* obj : objects_) {
		if (obj) obj->Update();
	}

	// UI スプライト更新
	if (gameOver_) gameOver_->Update();
	if (clear_) clear_->Update();
	if (EndUI_) EndUI_->Update();
	if (Black_) Black_->Update();
	if (explanation_) explanation_->Update();

	// 当たり判定
	if (player_ && enemy_) {
		float px, py, pz;
		player_->GetWorldPosition(px, py, pz);
		float ex, ey, ez;
		enemy_->GetWorldPosition(ex, ey, ez);
		const float radius = 0.5f + 0.5f;
		const float radiusSq = radius * radius;

		// Enemyの弾
		for (const auto& eb_up : enemy_->GetBullets()) {
			EnemyBullet* eb = eb_up.get();
			if (!eb || eb->IsDead()) continue;
			float bx, by, bz;
			eb->GetWorldPosition(bx, by, bz);
			float dx = bx - px;
			float dy = by - py;
			float dz = bz - pz;
			if (dx * dx + dy * dy + dz * dz <= radiusSq) {
				player_->OnCollision();
				eb->OnCollision();
				if (railCamera_) railCamera_->StartShake(0.12f, 0.5f);
			}
		}

		// Playerの弾
		for (const auto& pb_up : player_->GetBullets()) {
			PlayerBullet* pb = pb_up.get();
			if (!pb || pb->IsDead()) continue;
			float bx, by, bz;
			pb->GetWorldPosition(bx, by, bz);
			float dx = bx - ex;
			float dy = by - ey;
			float dz = bz - ez;
			if (dx * dx + dy * dy + dz * dz <= radiusSq) {
				enemy_->OnCollision();
				pb->OnCollision();
			}
		}

		// 弾同士の衝突判定
		for (const auto& pb_up : player_->GetBullets()) {
			PlayerBullet* pb = pb_up.get();
			if (!pb || pb->IsDead()) continue;
			float pax, pay, paz;
			pb->GetWorldPosition(pax, pay, paz);
			for (const auto& eb_up : enemy_->GetBullets()) {
				EnemyBullet* eb = eb_up.get();
				if (!eb || eb->IsDead()) continue;
				float ebx, eby, ebz;
				eb->GetWorldPosition(ebx, eby, ebz);
				float dx = ebx - pax;
				float dy = eby - pay;
				float dz = ebz - paz;
				if (dx * dx + dy * dy + dz * dz <= radiusSq) {
					eb->OnCollision();
					pb->OnCollision();
				}
			}
		}
	}

	// ゲームオーバー/クリア判定
	if (player_->IsDead()) {
		RequestStateChange(CreateGameOverState());
	}
	else if (enemy_->IsDead()) {
		if (particleManager_ == nullptr || particleManager_->IsEmpty()) {
			RequestStateChange(CreateClearState());
		}
	}
}

void GameScene::UpdateGameOver()
{
	if (input_->TriggerKey(DIK_RETURN) || input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_A)) {
		RequestStateChange(CreateReadyState());
		isGame_ = false;
		isSet_ = true;
	}
}

void GameScene::UpdateClear()
{
	if (input_->TriggerKey(DIK_RETURN) || input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_A)) {
		RequestStateChange(CreateReadyState());
		isGame_ = false;
		isSet_ = true;
	}
}

void GameScene::DrawReady()
{
	spriteCommon_->SettingCommonDraw();
	Ready_->Draw();
}

void GameScene::DrawGo()
{
	spriteCommon_->SettingCommonDraw();
	Go_->Draw();
}

void GameScene::DrawMain()
{
	object3dCommon_->SettingCommonDraw();
	// GameObject一括描画
	for (GameObject* obj : objects_) {
		if (obj) obj->Draw();
	}
	// パーティクル描画
	if (particleManager_) {
		particleManager_->Draw();
	}

	spriteCommon_->SettingCommonDraw();
	if(poose_->IsActive()) {
		poose_->Draw();
	}
	explanation_->Draw();
	// プレイヤーは別扱い
	player_->SpriteDraw();
}

void GameScene::DrawGameOver()
{
	spriteCommon_->SettingCommonDraw();
	gameOver_->Draw();
	EndUI_->Draw();
}

void GameScene::DrawClear()
{
	spriteCommon_->SettingCommonDraw();
	clear_->Draw();
	EndUI_->Draw();
}

void GameScene::Finalize()
{
	// マウスポインターが非表示のままなら復帰させる
	if (cursorHidden_) {
		ShowCursor(TRUE);
		cursorHidden_ = false;
	}

	// unique_ptr のリセットで自動解放
	player_.reset();
	enemy_.reset();
	skyDome_.reset();
	particleManager_.reset();
	Ready_.reset();
	Go_.reset();
	gameOver_.reset();
	clear_.reset();
	EndUI_.reset();
	Black_.reset();
	explanation_.reset();
	poose_.reset();

	railCamera_ = nullptr;
	spriteCommon_ = nullptr;
	object3dCommon_ = nullptr;
	input_ = nullptr;

	// objects_ は生ポインタ参照のみなのでクリア
	objects_.clear();
}