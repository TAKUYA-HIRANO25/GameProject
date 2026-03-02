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
	winApp_ = WinApp::GetInstance();
	winApp_->Initialize();

	// DirectX 初期化（レンダラのセットアップ）
	dxCommon_ = DirectXCommon::GetInstance();
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
	TextureManager::GetInstance()->LoadTexture("resources/Player/Player.png");
	TextureManager::GetInstance()->LoadTexture("resources/Reticle.png");
	TextureManager::GetInstance()->LoadTexture("resources/Explanation.png");
	TextureManager::GetInstance()->LoadTexture("resources/GameBack.png");
	// 必要なモデルをロード
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	ModelManager::GetInstance()->LoadModel("box.obj");
	ModelManager::GetInstance()->LoadModel("Bullet.obj");
	ModelManager::GetInstance()->LoadModel("Particle.obj");
	ModelManager::GetInstance()->LoadModel("sphere.obj");
	ModelManager::GetInstance()->LoadModel("Player/Player.obj");
	ModelManager::GetInstance()->LoadModel("Enemy/Enemy.obj");
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

	// 少し前進して視点を強調する (例: z を -14 へ 2 秒で移動) と軽い揺れ
	{
		Transform target = railCamera_->GetTransform();
		target.translate.z = -14.0f; // カメラを手前に（-20 -> -14）
		// 少し上に上げたい場合: target.translate.y += 1.5f;
		railCamera_->StartCinematicMove(target, 2.0f); // 2 秒で移動
		// 起動時の振動（small）
		railCamera_->StartShake(0.08f, 1.0f); // 強さ 0.08、1 秒
	}

	// スプライト群（タイトル / UI / フェード etc.）の生成と初期化
	spriteCommon_ = SpriteCommon::GetInstance();
	
	// ゲーム初期フラグを明確に初期化
	isFade_ = false;
	endFade_ = false;
	isStart_ = false;
	isGo_ = false;
	isGame_ = false;
	isOver_ = false;
	isClear_ = false;
	reup_ = false;

	// タイトルシーンの生成と初期化
	titleScene_ = new TitleScene();
	titleScene_->Initialize(railCamera_);

	// ゲームシーンの生成と初期化
	gameScene_ = new GameScene();
	gameScene_->Initialize(railCamera_);

	// フェード処理オブジェクトの生成（Sprite と連動）
	fadeEffect_ = new Fade(spriteCommon_, "resources/Fade.png");

	{
		// 初期カメラ行列とオブジェクト行列を即時更新しておく（最初のフレームで視界外になるのを防ぐ）
		if (railCamera_) {
			// RailCamera::Update() は camera->Update() を呼ぶので初期同期用に一度実行
			railCamera_->Update();
		}
	}

	waitingPreStartCinematic_ = false;
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

#ifdef USE_IMGUI
	// ImGui フレーム開始はフレーム単位で一度だけ行う（全シーン共通）
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif

	// カメラ
	railCamera_->Update();

	switch (currentScene)
	{
	case MyGame::Scene::Title:
		titleScene_->Update();
		if(titleScene_->IsGameStartSelected()) {
			isFade_ = true;
		}
		if (fadeEffect_->IsShrinking()) {
			isScene_ = false;
			currentScene = Scene::Game;
			titleScene_->Finalize(); // タイトルシーンの終了処理をここで行う（タイトルからゲームへ遷移するタイミングで）
			gameScene_->Initialize(railCamera_); // ゲームシーンの初期化をここで行う（タイトルからゲームへ遷移するタイミングで）
		}
		break;
	case MyGame::Scene::Rule:

		break;
	case MyGame::Scene::Poose:

		break;
	case MyGame::Scene::Select:

		break;
	case MyGame::Scene::Game:
		gameScene_->Update();

		if (gameScene_->IsGameSet()) {
			currentScene = MyGame::Scene::Title;
			isScene_ = false;
			gameScene_->Finalize(); // ゲームシーンの終了処理をここで行う（ゲームからタイトルへ遷移するタイミングで）
			titleScene_->Initialize(railCamera_); // タイトルシーンの初期化をここで行う（ゲームからタイトルへ遷移するタイミングで）
		}
		// ゲームオーバー / クリアのフラグをゲームシーンから取得して反映
	default:

		break;
	}

#ifdef USE_IMGUI
	ImGui::Render();
#endif
	// フェード処理
	if (isFade_) {
		if (fadeEffect_ && !fadeEffect_->IsRunning() && !fadeEffect_->IsFinished()) {
			fadeEffect_->Start();
		}
		else if (fadeEffect_ && !fadeEffect_->IsRunning() && fadeEffect_->IsFinished() && isFade_) {
			fadeEffect_->Reset();
			fadeEffect_->Start();
		}

		if (fadeEffect_ && (fadeEffect_->IsRunning() || isFade_)) {
			fadeEffect_->Update();
		}

		if (fadeEffect_ && fadeEffect_->IsShrinking() && !endFade_) {
			endFade_ = true;
		}

		if (fadeEffect_ && fadeEffect_->IsFinished()) {
			endFade_ = false;
			isFade_ = false;
			isScene_ = true;
			fadeEffect_->Reset();
		}
	}
}


void MyGame::Draw()
{
	// 描画開始（コマンドリスト等の初期化）
	dxCommon_->PreDraw();

	switch (currentScene)
	{
		case MyGame::Scene::Title:
		titleScene_->Draw();
		break;

		case MyGame::Scene::Rule:
			break;

		case MyGame::Scene::Poose:
			break;

		case MyGame::Scene::Select:
			break;

		case MyGame::Scene::Game:
			gameScene_->Draw();
			break;
	}
	spriteCommon_->SettingCommonDraw();
	fadeEffect_->Draw();

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


#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

}

void MyGame::ReleaseResources()
{
	delete railCamera_; railCamera_ = nullptr;
	delete titleScene_; titleScene_ = nullptr;
	delete gameScene_; gameScene_ = nullptr;

	// modelCommon_ は Initialize で new しているため解放
	delete modelCommon_; modelCommon_ = nullptr;
}