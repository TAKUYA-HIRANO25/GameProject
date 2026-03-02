#include "GameScene.h"

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void GameScene::Initialize(RailCamera* railCamera)
{
	// WinAPI 初期化
	winApp_ = WinApp::GetInstance();
	// DirectX 初期化
	dxCommon_ = DirectXCommon::GetInstance();
	// 入力システムの取得・初期化
	input_ = Input::GetInstance();
	// テクスチャ管理・スプライト共通設定の初期化
	spriteCommon_ = SpriteCommon::GetInstance();
	object3dCommon_ = ObJect3dCommon::GetInstance();

	// パーティクルマネージャ生成（3D 共通設定を渡す）
	particleManager_ = new ParticleManager(object3dCommon_);

	// カメラ
	railCamera_ = railCamera;
	// プレイヤー生成・初期化
	player_ = new Player();
	player_->Initialize(object3dCommon_);
	player_->SetRailCameraVelocity(railCamera_->GetVelocity());
	player_->SetParticleManager(particleManager_);

	// 敵生成・初期化
	enemy_ = new Enemy();
	enemy_->Initialize(object3dCommon_, Vector3{ 0.0f, -1.0f, 60.0f });
	enemy_->setPlayer(player_);
	enemy_->SetParticleManager(particleManager_);

	// 天球 (sky dome) の初期化
	skyDome_ = new Object3d();
	skyDome_->Initialize(object3dCommon_);
	skyDome_->SetModel("sphere.obj");

	// スプライトの初期化
	Ready_ = new Sprite();
	Ready_->Initialize(spriteCommon_, "resources/Ready.png");
	Go_ = new Sprite();
	Go_->Initialize(spriteCommon_, "resources/GO.png");
	gameOver_ = new Sprite();
	gameOver_->Initialize(spriteCommon_, "resources/GameOver.png");
	clear_ = new Sprite();
	clear_->Initialize(spriteCommon_, "resources/Clear.png");
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
}

void GameScene::Update()
{
	switch (currentScene_)
	{
	case GameScene::Scene::ready:
		++startTime_;
		Ready_->Update();
		if (startTime_ >= 220) {
			currentScene_ = Scene::Go;
		}
		break;
	case GameScene::Scene::Go:
		++goTime_;
		Go_->Update();
		if (goTime_ >= 120) {
			currentScene_ = Scene::main;
		}
		break;
	case GameScene::Scene::main:
		if (input_->TriggerKey(DIK_F)) {
			if (!poose_->IsActive()) {
				poose_->Activate();
			}
			else {
				poose_->Deactivate();
			}
		}
		// ポーズが有効なら Poose を先に更新し、結果が出たら処理する（ゲーム本体の更新はスキップ）
		if (poose_->IsActive()) {
			poose_->Update();

			auto r = poose_->GetResult();
			if (r != Poose::Result::None) {
				if (r == Poose::Result::Resume) {
					poose_->Deactivate();
				}
				else if (r == Poose::Result::ToTitle) {
					poose_->Deactivate();
				}
				poose_->ClearResult();
			}

			// ポーズ中はゲームロジック更新をスキップしてフェード処理へ
			goto SKIP_GAME_UPDATE;
		}
		// プレイヤー更新
		player_->Update();
		// 敵更新
		enemy_->Update();
		// パーティクル更新（描画用のエミッタ更新など）
		particleManager_->Update();
		// UI スプライト更新（タイトル外で常に更新）
		gameOver_->Update();
		clear_->Update();
		Black_->Update();
		explanation_->Update();

		// 当たり判定（プレイヤー・敵・弾の座標比較）
		if (player_ && enemy_) {
			Vector3 posA = player_->GetWorldPosition();
			const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
			const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

			for (EnemyBullet* bullet : enemyBullets) {
				Vector3 posB = bullet->GetWorldPosition();
				float coll = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z);
				if (coll <= (0.5f + 0.5f) * (0.5f + 0.5f)) {
					// プレイヤーにヒット
					player_->OnCollision();
					bullet->OnCollision();

					// カメラ揺れを発生（存在チェック）
					if (railCamera_) {
						railCamera_->StartShake(0.12f, 0.5f);
					}
				}
			}

			posA = enemy_->GetWorldPosition();
			for (PlayerBullet* bullet : playerBullets) {
				Vector3 posB = bullet->GetWorldPosition();
				float coll = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z);
				if (coll <= (0.5f + 0.5f) * (0.5f + 0.5f)) {
					// 敵にヒット
					enemy_->OnCollision();
					bullet->OnCollision();
				}
			}

			// 弾同士の衝突判定
			for (PlayerBullet* pb : playerBullets) {
				Vector3 a = pb->GetWorldPosition();
				for (EnemyBullet* eb : enemyBullets) {
					Vector3 b = eb->GetWorldPosition();
					float coll = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y) + (b.z - a.z) * (b.z - a.z);
					if (coll <= (0.5f + 0.5f) * (0.5f + 0.5f)) {
						eb->OnCollision();
						pb->OnCollision();
					}
				}
			}
		}

		// ゲームオーバー / クリア判定
		if (player_->IsDead()) {
			currentScene_ = Scene::gameOver;
		}
		else if (enemy_->IsDead()) {
			currentScene_ = Scene::clear;
		}
	SKIP_GAME_UPDATE:

		break;
	case GameScene::Scene::gameOver:
		if (input_->TriggerKey(DIK_T) || input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_A)) {
			currentScene_ = Scene::ready;
			isGame_ = false;
			isSet_ = true;
		}
		break;
	case GameScene::Scene::clear:
		if (input_->TriggerKey(DIK_T) || input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_A)) {
			currentScene_ = Scene::ready;
		 isGame_ = false;
		 isSet_ = true;
		}
		break;
	default:
		break;
	}

	skyDome_->Updata();

	// imgui フレームは MyGame::Update() で開始されるため、ここではウィジェット作成のみ行う
#ifdef USE_IMGUI
	ImGui::ShowDemoWindow();

	// プレイヤー変換編集ウィンドウ
	if (player_) {
		// 現在値を取得
		Vector3 pos = player_->GetWorldPosition();
		Vector3 sc = player_->GetScale();
		Vector3 rt = player_->GetRotate();

		float p[3] = { pos.x, pos.y, pos.z };
		float s[3] = { sc.x, sc.y, sc.z };
		float r[3] = { rt.x, rt.y, rt.z };

		ImGui::Begin("Player Transform");
		if (ImGui::DragFloat3("Position", p, 0.1f, -100.0f, 100.0f)) {
			player_->SetPosition({ p[0], p[1], p[2] });
		}
		if (ImGui::DragFloat3("Scale", s, 0.01f, 0.001f, 20.0f)) {
			player_->SetScale({ s[0], s[1], s[2] });
		}
		if (ImGui::DragFloat3("Rotation", r, 0.01f, -6.28318f, 6.28318f)) {
			player_->SetRotate({ r[0], r[1], r[2] });
		}
		ImGui::Text("Tip: Rotation in radians.");
		ImGui::End();
	}
#endif
}

void GameScene::Draw()
{
	switch (currentScene_) {
		case GameScene::Scene::ready:
			spriteCommon_->SettingCommonDraw();
			Ready_->Draw();
			break;

		case GameScene::Scene::Go:
			spriteCommon_->SettingCommonDraw();
			Go_->Draw();
			break;

		case GameScene::Scene::main:
			object3dCommon_->SettingCommonDraw();
			// プレイヤー描画
			if (player_) {
				player_->Draw();
			}
			// 敵描画
			if (enemy_) {
				enemy_->Draw();
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
			player_->SpriteDraw();
			break;

		case GameScene::Scene::gameOver:
			spriteCommon_->SettingCommonDraw();
			gameOver_->Draw();
			break;
		case GameScene::Scene::clear:
			spriteCommon_->SettingCommonDraw();
			clear_->Draw();
			break;

		default:
			break;
	}

	object3dCommon_->SettingCommonDraw();
	// 天球描画
	if (skyDome_) {
		skyDome_->Draw();
	}
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
	delete Black_;
	Black_ = nullptr;
	delete explanation_;
	explanation_ = nullptr;
	delete poose_;
	poose_ = nullptr;
	railCamera_ = nullptr;
	// シングルトン参照は解放しない（グローバル管理）
	spriteCommon_ = nullptr;
	object3dCommon_ = nullptr;
	input_ = nullptr;
}