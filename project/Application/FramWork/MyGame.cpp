#include "MyGame.h"
#include <cassert>
#include "TextureManager.h"
#include "ModelManager.h"
#include "Input.h"
#include "SpriteCommon.h"
#include "Object3dCommon.h"

// Initialize: main.cpp の初期化処理を移植（詳細を可能な限り再現）
void MyGame::Initialize()
{
	// WinAPI
	winApp_ = new WinApp();
	winApp_->Initialize();

	// DirectX
	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialize(winApp_);

	// Input シングルトン取得（既存実装に合わせる）
	input_ = Input::GetInstance();
	input_->Initialize(winApp_);

	// TextureManager 初期化
	TextureManager::GetInstance()->Initialize(dxCommon_);

	// SpriteCommon（シングルトン）
	spriteCommon_ = SpriteCommon::GetInstance();
	spriteCommon_->Initialize(dxCommon_);

	// ModelCommon（ローダ等）
	modelCommon_ = new ModelCommon();
	modelCommon_->Initialize(dxCommon_);

	// Object3dCommon（シングルトン）
	object3dCommon_ = ObJect3dCommon::GetInstance();
	object3dCommon_->Initialize(dxCommon_);


	// ModelManager 初期化
	ModelManager::GetInstance()->Initialize(dxCommon_);

	// 主要テクスチャ・モデルを読み込み（main.cpp と同様）
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resources/title.png");
	TextureManager::GetInstance()->LoadTexture("resources/titleUI.png");
	TextureManager::GetInstance()->LoadTexture("resources/backGround.png");
	TextureManager::GetInstance()->LoadTexture("resources/Fade.png");
	TextureManager::GetInstance()->LoadTexture("resources/Ready.png");
	TextureManager::GetInstance()->LoadTexture("resources/GO.png");
	TextureManager::GetInstance()->LoadTexture("resources/GameOver.png");
	TextureManager::GetInstance()->LoadTexture("resources/Over.png");
	TextureManager::GetInstance()->LoadTexture("resources/Clear.png");
	TextureManager::GetInstance()->LoadTexture("resources/Particle.png");
	TextureManager::GetInstance()->LoadTexture("resources/sky.png");

	// モデル読み込み
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	ModelManager::GetInstance()->LoadModel("box.obj");
	ModelManager::GetInstance()->LoadModel("Bullet.obj");
	ModelManager::GetInstance()->LoadModel("Particle.obj");
	ModelManager::GetInstance()->LoadModel("sphere.obj");

	// シーン構築（カメラ、プレイヤー、敵、パーティクル、スプライト）
	CreateScene();
}

// シーン作成（main.cpp の処理を移植）
void MyGame::CreateScene()
{
	// カメラ / レールカメラ
	Camera* camera = new Camera;
	camera->SetRotate({ 0.0f, 6.28f, 0.0f });
	camera->SetTranslate({ 0.0f, 0.0f, -20.0f });

	railCamera_ = new RailCamera();
	railCamera_->Initialize(camera);
	if (object3dCommon_) object3dCommon_->SetDefaultCamera(railCamera_->GetCamera());

	// パーティクル
	particleManager_ = new ParticleManager(object3dCommon_);

	// プレイヤー
	player_ = new Player();
	player_->Initialize(object3dCommon_, input_);
	player_->SetRailCameraVelocity(railCamera_->GetVelocity());
	player_->SetParticleManager(particleManager_);

	// 敵
	enemy_ = new Enemy();
	enemy_->Initialize(object3dCommon_, Vector3{ 0.0f, -4.0f, 40.0f });
	enemy_->setPlayer(player_);
	enemy_->SetParticleManager(particleManager_);

	// 天球（sky dome）
	skyDome_ = new Object3d();
	skyDome_->Initialize(object3dCommon_);
	skyDome_->SetModel("sphere.obj");

	// スプライト群（タイトル / UI / フェード etc.）
	spriteCommon_ = SpriteCommon::GetInstance();
	title_ = new Sprite(); title_->Initialize(spriteCommon_, "resources/title.png");
	titleUI_ = new Sprite(); titleUI_->Initialize(spriteCommon_, "resources/titleUI.png");
	backGround_ = new Sprite(); backGround_->Initialize(spriteCommon_, "resources/backGround.png"); backGround_->SetSize(Vector2(1280, 720));
	fadeSprite_ = new Sprite(); fadeSprite_->Initialize(spriteCommon_, "resources/Fade.png"); fadeSprite_->SetSize(Vector2(0, 0)); fadeSprite_->SetPosition(Vector2(630, 360));
	Ready_ = new Sprite(); Ready_->Initialize(spriteCommon_, "resources/Ready.png");
	Go_ = new Sprite(); Go_->Initialize(spriteCommon_, "resources/GO.png");
	gameOver_ = new Sprite(); gameOver_->Initialize(spriteCommon_, "resources/GameOver.png");
	clear_ = new Sprite(); clear_->Initialize(spriteCommon_, "resources/Clear.png");
	Black_ = new Sprite(); Black_->Initialize(spriteCommon_, "resources/backGround.png"); Black_->SetSize(Vector2(1280, 720)); Black_->SetColor(Vector4(1, 1, 1, 0));

	// 初期状態
	isTitle_ = true;
	isFade_ = false;
	endFade_ = false;
	isStart_ = false;
	isGo_ = false;
	isGame_ = false;
	isOver_ = false;
	isClear_ = false;
	reup_ = false;
	titleTime_ = 0;
	startTime_ = 0;
	goTime_ = 0;

	fadeEffect_ = new Fade(spriteCommon_, "resources/Fade.png");
}

// 毎フレーム更新: main.cpp のループを移植
void MyGame::Update()
{
	// ウィンドウメッセージ処理（最初に行う
	if (winApp_->ProcessMessage()) {
		roopOut_ = true;
		return;
	}

	// 入力更新はメッセージ処理の直後に行う
	input_->Update();

	// imgui フレームは入力更新の後に作る
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// UI を構築
	ImGui::ShowDemoWindow();
	// ... 他の ImGui コントロール ...
	// 描画用に Render を呼ぶ（描画直前に呼んでも OK）
	ImGui::Render();
#endif

	// 以下、既存の更新処理（ゲームロジック等）を続ける
	// 3D 共通設定
	object3dCommon_->SettingCommonDraw();

	// マウスボタンの簡易取得（必要なら Input の API を使う）
	if (input_->PushMouse(0)) 
	{ 
		mouseLeft_ = true; 
	}
	else if (input_->PushMouse(1))
	{
		mouseRight_ = true;
	}

	// 状態遷移と更新
	if (isTitle_) {
		title_->Update();
		titleUI_->Update();
		backGround_->Update();
		if (reup_) {
			// 再初期化（main.cpp と同様の動作）
			player_->Initialize(object3dCommon_, input_);
			enemy_->Initialize(object3dCommon_, Vector3{ 0.0f, -4.0f, 40.0f });
			reup_ = false;
		}
		// 入力でフェード開始
		if (input_->TriggerKey(DIK_RETURN)) {
			isFade_ = true;
			isOver_ = false;
		}
	}
	else {
		// スタート / Go フラグ処理
		if (isStart_) {
			++startTime_;
			if (startTime_ >= 120) {
				isStart_ = false;
				isGo_ = true;
			}
		}
		if (isGo_) {
			++goTime_;
			if (goTime_ >= 120) {
				isGo_ = false;
				isGame_ = true;
			}
		}

		// ゲーム本体更新
		if (isGame_) {
			railCamera_->Update();
			if (!isOver_ && !isClear_) {
				player_->Update();
				enemy_->Update();
			}


			// 当たり判定（main.cpp の判定を移植）
			if (player_ && enemy_) {
				Vector3 posA = player_->GetWorldPosition();
				const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
				const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

				for (EnemyBullet* bullet : enemyBullets) {
					Vector3 posB = bullet->GetWorldPosition();
					float coll = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z);
					if (coll <= (0.5f + 0.5f) * (0.5f + 0.5f)) {
						player_->OnCollision();
						bullet->OnCollision();
					}
				}

				posA = enemy_->GetWorldPosition();
				for (PlayerBullet* bullet : playerBullets) {
					Vector3 posB = bullet->GetWorldPosition();
					float coll = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z);
					if (coll <= (0.5f + 0.5f) * (0.5f + 0.5f)) {
						enemy_->OnCollision();
						bullet->OnCollision();
					}
				}

				// 弾同士の衝突
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

			// パーティクル更新
			particleManager_->Update();
		}

		// UI スプライト更新
		Ready_->Update();
		Go_->Update();
		gameOver_->Update();
		clear_->Update();
		Black_->Update();
	}

#ifdef USE_IMGUI
	ImGui::Render();
#endif 

	// フェード処理
	if (isFade_) {
		if (fadeEffect_ && !fadeEffect_->IsRunning() && !fadeEffect_->IsFinished()) {
			fadeEffect_->Start();
		}
		if (fadeEffect_ && (fadeEffect_->IsRunning() || isFade_)) {
			fadeEffect_->Update();
		}
		if (fadeEffect_ && fadeEffect_->IsShrinking() && !endFade_) {
			isTitle_ = false;
			endFade_ = true;
		}
		if (fadeEffect_ && fadeEffect_->IsFinished()) {
			endFade_ = false;
			isFade_ = false;
			isStart_ = true; // スタート演出へ移行する例
			startTime_ = 0;
			goTime_ = 0;
			fadeEffect_->Reset();
		}
	}
}

// 描画（main.cpp の描画処理を移植）
void MyGame::Draw()
{
	// 描画開始
	dxCommon_->PreDraw();

	// スプライト用 PSO 設定（背景/UI）
	spriteCommon_->SettingCommonDraw();

	if (isTitle_) {
		backGround_->Draw();
		title_->Draw();
		titleUI_->Draw();
	}
	else {
		// 3D 描画: Object3dCommon の PSO を設定してから描画
		object3dCommon_->SettingCommonDraw();

		// skyDome
		skyDome_->Draw();

		// ゲームオブジェクト
		if (!isOver_)
		{
			player_->Draw();
		}
		if (!isClear_)
		{
			enemy_->Draw();
		}

		// スタート / GO 表示
		if (isStart_)
		{
			Ready_->Draw();
		}
		else if (isGo_)
		{
			Go_->Draw();
		}

		// ゲームオーバー / クリア UI
		if (isOver_) {
			// スプライト用 PSO に戻して描画
			spriteCommon_->SettingCommonDraw();
			Black_->Draw();
			gameOver_->Draw();
		}
		if (isClear_) {
			spriteCommon_->SettingCommonDraw();
			Black_->Draw();
			clear_->Draw();
		}

		// パーティクル
		particleManager_->Draw();

		// フェードや UI はスプライトPSOに戻して描画
		spriteCommon_->SettingCommonDraw();

	}

	// フェードはスプライトとして描画（spriteCommon_ が設定された状態で呼ぶ）
	if (fadeEffect_ && (isFade_ || endFade_ || fadeEffect_->IsRunning())) {
		// spriteCommon_->SettingCommonDraw() が既に呼ばれていることを前提
		fadeEffect_->Draw();
	}

#ifdef USE_IMGUI
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());
#endif

	dxCommon_->PostDrow();
}

// Finalize: main.cpp の解放処理を移植
void MyGame::Finalize()
{
	ReleaseResources();

	// singletons の Finalize 呼び出し
	ModelManager::GetInstance()->Finalize();
	TextureManager::GetInstance()->Finalize();

	// DirectX/WinApp 解放
	if (dxCommon_) { delete dxCommon_; dxCommon_ = nullptr; }
	if (winApp_) { winApp_->Finalize(); delete winApp_; winApp_ = nullptr; }

#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif 

}

// リソース解放
void MyGame::ReleaseResources()
{
	delete title_; title_ = nullptr;
	delete titleUI_; titleUI_ = nullptr;
	delete backGround_; backGround_ = nullptr;
	delete fadeSprite_; fadeSprite_ = nullptr;
	delete Ready_; Ready_ = nullptr;
	delete Go_; Go_ = nullptr;
	delete Black_; Black_ = nullptr;
	delete gameOver_; gameOver_ = nullptr;
	delete clear_; clear_ = nullptr;

	delete player_; player_ = nullptr;
	delete enemy_; enemy_ = nullptr;
	delete particleManager_; particleManager_ = nullptr;
	delete railCamera_; railCamera_ = nullptr;
	delete skyDome_; skyDome_ = nullptr;

	delete modelCommon_; modelCommon_ = nullptr;
}