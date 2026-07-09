#include "GameScene.h"
#include "GameSceneState.h"
#include <windows.h>
#include <cstdio>

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void GameScene::Initialize(RailCamera* railCamera)
{
	// WinAPI初期化
	winApp_ = WinApp::GetInstance();
	// DirectX初期化
	dxCommon_ = DirectXCommon::GetInstance();
	// 入力システムの取得、初期化
	input_ = Input::GetInstance();
	// テクスチャ管理、スプライト共通設定の初期化
	spriteCommon_ = SpriteCommon::GetInstance();
	object3dCommon_ = ObJect3dCommon::GetInstance();

	// パーティクルマネージャ生成
	particleManager_ = new ParticleManager(object3dCommon_);

	// カメラ
	railCamera_ = railCamera;
	// プレイヤー生成、初期化
	player_ = new Player();
	player_->Initialize(object3dCommon_);
	player_->SetRailCameraVelocity(railCamera_->GetVelocity());
	player_->SetParticleManager(particleManager_);

	// 敵生成、初期化
	enemy_ = new Enemy();
	enemy_->Initialize(object3dCommon_, Vector3{ 0.0f, -1.0f, 60.0f });
	enemy_->setPlayer(player_);
	enemy_->SetParticleManager(particleManager_);

	// GameObject一括管理リストに登録
	objects_.clear();
	objects_.push_back(static_cast<GameObject*>(player_));
	objects_.push_back(static_cast<GameObject*>(enemy_));

	// 天球の初期化
	skyDome_ = new Object3d();
	skyDome_->Initialize(object3dCommon_);
	skyDome_->SetModel("sphere.obj");
	skyDome_->SetScale({ 50.0f, 50.0f, 50.0f });
	skyDome_->SetTranslate({ 0.0f, -1.0f, 20.0f });

	// スプライトの初期化
	Ready_ = new Sprite();
	Ready_->Initialize(spriteCommon_, "resources/Ready.png");
	Go_ = new Sprite();
	Go_->Initialize(spriteCommon_, "resources/GO.png");
	gameOver_ = new Sprite();
	gameOver_->Initialize(spriteCommon_, "resources/GameOver.png");
	clear_ = new Sprite();
	clear_->Initialize(spriteCommon_, "resources/GameClear.png");
	EndUI_ = new Sprite();
	EndUI_->Initialize(spriteCommon_, "resources/EndUI.png");
	Black_ = new Sprite();
	Black_->Initialize(spriteCommon_, "resources/backGround.png"); Black_->SetSize(Vector2(1280, 720)); Black_->SetColor(Vector4(1, 1, 1, 0));
	explanation_ = new Sprite();
	explanation_->Initialize(spriteCommon_, "resources/Explanation.png");

	// Poose の初期化
	poose_ = new Poose();
	poose_->Initialize();

	//フラグ
	isGame_ = true;
	isSet_ = false;
	// タイマー初期化
	startTime_ = 0;
	goTime_ = 0;

	// 現在のシーン
	Scene currentScene_ = Scene::ready;

	// Stateパターン:初期状態をセット
	state_ = CreateReadyState();
	if (state_) state_->Enter(this);

	
}

void GameScene::Update()
{
	// pendingStateがセットされていればここで適用
	if (pendingState_) {
		if (state_) state_->Exit(this);
		state_ = std::move(pendingState_);
		if (state_) state_->Enter(this);
	}

	if (state_) {
		state_->Update(this);
		return;
	}

	skyDome_->Updata();

#ifdef USE_IMGUI
	
	ImGui::ShowDemoWindow();
	
#endif
}

void GameScene::Draw()
{
	// pendingState を反映
	if (pendingState_) {
		if (state_) state_->Exit(this);
		state_ = std::move(pendingState_);
		if (state_) state_->Enter(this);
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
	// パーティクル更新
	if (particleManager_) particleManager_->Update();

	// GameObject 一括 Update(Player/Enemy等)
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

		for (EnemyBullet* eb : enemy_->GetBullets()) {
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

		for (PlayerBullet* pb : player_->GetBullets()) {
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

		for (PlayerBullet* pb : player_->GetBullets()) {
			if (!pb || pb->IsDead()) continue;
			float pax, pay, paz;
			pb->GetWorldPosition(pax, pay, paz);
			for (EnemyBullet* eb : enemy_->GetBullets()) {
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
	// GameObject一括描画(Player/Enemy等)
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
	// プレイヤーのスプライトは別扱い
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
	delete player_;
	player_ = nullptr;
	delete enemy_;
	enemy_ = nullptr;
	delete skyDome_;
	skyDome_ = nullptr;
	delete particleManager_;
	particleManager_ = nullptr;
	delete Ready_;
	Ready_ = nullptr;
	delete Go_;
	Go_ = nullptr;
	delete gameOver_;
	gameOver_ = nullptr;
	delete clear_;
	clear_ = nullptr;
	delete EndUI_;
	EndUI_ = nullptr;
	delete Black_;
	Black_ = nullptr;
	delete explanation_;
	explanation_ = nullptr;
	delete poose_;
	poose_ = nullptr;
	railCamera_ = nullptr;
	// シングルトン参照は解放しない
	spriteCommon_ = nullptr;
	object3dCommon_ = nullptr;
	input_ = nullptr;

	// objects_ は生ポインタ参照のみなのでクリア
	objects_.clear();
}