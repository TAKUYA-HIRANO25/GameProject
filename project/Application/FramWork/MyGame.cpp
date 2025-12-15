#include "MyGame.h"
#include <cassert>
#include "TextureManager.h"
#include "ModelManager.h"
#include "Input.h"
#include "SpriteCommon.h"
#include "Object3dCommon.h"

// MyGame:
// - アプリケーション固有の初期化 / 更新 / 描画 / 解放処理を実装する。
// - main.cpp のループを Framework::Run から移植した形。
// - 本ファイルではシーン作成、入力反応、ゲーム状態の遷移、フェード制御などを扱う。

void MyGame::Initialize()
{
	// WinAPI 初期化（ウィンドウ生成とメッセージループ準備）
	winApp_ = new WinApp();
	winApp_->Initialize();

	// DirectX 初期化（レンダラのセットアップ）
	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialize(winApp_);

	// 入力システムの取得・初期化（シングルトン想定）
	input_ = Input::GetInstance();
	input_->Initialize(winApp_);

	// テクスチャ管理・スプライト共通設定の初期化
	TextureManager::GetInstance()->Initialize(dxCommon_);
	spriteCommon_ = SpriteCommon::GetInstance();
	spriteCommon_->Initialize(dxCommon_);

	// モデル周りの初期化
	modelCommon_ = new ModelCommon();
	modelCommon_->Initialize(dxCommon_);

	// 3D 共通設定（カメラなど）初期化
	object3dCommon_ = ObJect3dCommon::GetInstance();
	object3dCommon_->Initialize(dxCommon_);

	// モデルマネージャ初期化（必要なモデルをロードする準備）
	ModelManager::GetInstance()->Initialize(dxCommon_);

	// 主要テクスチャ・モデルを読み込み（リソースを事前ロードしておく）
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

	// 必要なモデルをロード
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	ModelManager::GetInstance()->LoadModel("box.obj");
	ModelManager::GetInstance()->LoadModel("Bullet.obj");
	ModelManager::GetInstance()->LoadModel("Particle.obj");
	ModelManager::GetInstance()->LoadModel("sphere.obj");

	// シーン構築（カメラ、プレイヤー、敵、パーティクル、スプライト）
	CreateScene();
}

void MyGame::CreateScene()
{
	// カメラ / レールカメラの作成と初期配置
	Camera* camera = new Camera;
	camera->SetRotate({ 0.0f, 6.28f, 0.0f });
	camera->SetTranslate({ 0.0f, 0.0f, -20.0f });

	railCamera_ = new RailCamera();
	railCamera_->Initialize(camera);
	if (object3dCommon_) object3dCommon_->SetDefaultCamera(railCamera_->GetCamera());

	// パーティクルマネージャ生成（3D 共通設定を渡す）
	particleManager_ = new ParticleManager(object3dCommon_);

	// プレイヤー生成・初期化
	player_ = new Player();
	player_->Initialize(object3dCommon_, input_);
	player_->SetRailCameraVelocity(railCamera_->GetVelocity());
	player_->SetParticleManager(particleManager_);

	// 敵生成・初期化
	enemy_ = new Enemy();
	enemy_->Initialize(object3dCommon_, Vector3{ 0.0f, -4.0f, 40.0f });
	enemy_->setPlayer(player_);
	enemy_->SetParticleManager(particleManager_);

	// 天球 (sky dome) の初期化
	skyDome_ = new Object3d();
	skyDome_->Initialize(object3dCommon_);
	skyDome_->SetModel("sphere.obj");

	// スプライト群（タイトル / UI / フェード etc.）の生成と初期化
	spriteCommon_ = SpriteCommon::GetInstance();
	title_ = new Sprite(); 
	title_->Initialize(spriteCommon_, "resources/title.png");
	titleUI_ = new Sprite(); 
	titleUI_->Initialize(spriteCommon_, "resources/titleUI.png");
	backGround_ = new Sprite(); 
	backGround_->Initialize(spriteCommon_, "resources/backGround.png"); backGround_->SetSize(Vector2(1280, 720));
	fadeSprite_ = new Sprite(); 
	fadeSprite_->Initialize(spriteCommon_, "resources/Fade.png"); fadeSprite_->SetSize(Vector2(0, 0)); fadeSprite_->SetPosition(Vector2(630, 360));
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

	// ゲーム初期フラグを明確に初期化
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

	// フェード処理オブジェクトの生成（Sprite と連動）
	fadeEffect_ = new Fade(spriteCommon_, "resources/Fade.png");
}

void MyGame::Update()
{
	// ウィンドウメッセージ処理（最初に行う）
	if (winApp_->ProcessMessage()) {
		roopOut_ = true;
		return;
	}

	// 入力更新はメッセージ処理の直後に行う
	input_->Update();

	// imgui フレームは入力更新の後に作る（デバッグ UI）
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	// 描画用に Render を呼ぶ（描画直前に呼んでも OK）
	ImGui::Render();
#endif

	// 3D 共通描画用設定（シェーダ / PSO 設定など）
	object3dCommon_->SettingCommonDraw();

	// 簡易マウス入力のフラグ管理
	if (input_->PushMouse(0)) 
	{ 
		mouseLeft_ = true; 
	}
	else if (input_->PushMouse(1))
	{
		mouseRight_ = true;
	}

	// シーンの状態に応じた更新
	if (isTitle_) {
		// タイトル画面専用の UI 更新
		title_->Update();
		titleUI_->Update();
		backGround_->Update();

		// 再初期化要求があればオブジェクトの初期化を行う
		if (reup_) {
			player_->Initialize(object3dCommon_, input_);
			enemy_->Initialize(object3dCommon_, Vector3{ 0.0f, -4.0f, 40.0f });
			reup_ = false;
		}

		// Enter キーでフェード開始フラグを立てる
		if (input_->TriggerKey(DIK_RETURN)) {
			isFade_ = true;
			isOver_ = false;
		}
	}
	else {
		// スタート / Go フラグ処理（カウントでフラグを切り替える）
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

		// ゲーム本体の更新（カメラ / プレイヤー / 敵 / 当たり判定）
		if (isGame_) {
			railCamera_->Update();
			if (!isOver_ && !isClear_) {
				player_->Update();
				enemy_->Update();
			}

			// 当たり判定（プレイヤー・敵・弾の座標比較）
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

			// パーティクル更新（描画用のエミッタ更新など）
			particleManager_->Update();
		}

		// UI スプライト更新（タイトル外で常に更新）
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
	// - isFade_ が true のときに Fade オブジェクトを開始/更新し、縮小開始時にタイトルを解除する
	if (isFade_) {
		// フェード開始条件：fadeEffect_ が存在し、実行中でなく、かつ未完了の場合
		if (fadeEffect_ && !fadeEffect_->IsRunning() && !fadeEffect_->IsFinished()) {
			fadeEffect_->Start();
		}
		// フェードの毎フレーム更新
		if (fadeEffect_ && (fadeEffect_->IsRunning() || isFade_)) {
			fadeEffect_->Update();
		}
		// フェードが縮小フェーズに入ったらタイトルを非表示にする（1回だけ実行）
		if (fadeEffect_ && fadeEffect_->IsShrinking() && !endFade_) {
			isTitle_ = false;
			endFade_ = true;
		}
		// フェード完了時の遷移処理
		if (fadeEffect_ && fadeEffect_->IsFinished()) {
			endFade_ = false;
			isFade_ = false;
			isStart_ = true; // スタート演出へ移行	
			startTime_ = 0;
			goTime_ = 0;
			fadeEffect_->Reset();
		}
	}
}

void MyGame::Draw()
{
	// 描画開始（コマンドリスト等の初期化）
	dxCommon_->PreDraw();

	// スプライト用 PSO 設定（背景/UI 用）
	spriteCommon_->SettingCommonDraw();

	if (isTitle_) {
		// タイトル画面描画（背景・タイトル・UI）
		backGround_->Draw();
		title_->Draw();
		titleUI_->Draw();
	}
	else {
		// 3D 描画: Object3dCommon の PSO を設定してから 3D オブジェクトを描画する
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

		// ゲームオーバー / クリア UI（スプライト PSO に戻して描画）
		if (isOver_) {
			spriteCommon_->SettingCommonDraw();
			Black_->Draw();
			gameOver_->Draw();
		}
		if (isClear_) {
			spriteCommon_->SettingCommonDraw();
			Black_->Draw();
			clear_->Draw();
		}

		// パーティクル描画（3D）
		particleManager_->Draw();

		// フェードや UI はスプライト PSO に戻して描画する
		spriteCommon_->SettingCommonDraw();

	}

	// フェードはスプライトとして描画（spriteCommon_ が設定された状態で呼ぶ）
	// - フェードが存在しており、何らかの表示条件が満たされている場合に描画
	if (fadeEffect_ && (isFade_ || endFade_ || fadeEffect_->IsRunning())) {
		// spriteCommon_->SettingCommonDraw() が既に呼ばれていることを前提
		fadeEffect_->Draw();
	}

#ifdef USE_IMGUI
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());
#endif

	// 描画終了（Present 等）
	dxCommon_->PostDrow();
}

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

	// modelCommon_ は Initialize で new しているため解放
	delete modelCommon_; modelCommon_ = nullptr;
}