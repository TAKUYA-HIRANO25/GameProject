#include "TitleScene.h"


TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
    
}

void TitleScene::Initialize(RailCamera* railCamera)
{
    // WinAPI初期化
    winApp_ = WinApp::GetInstance();

    // DirectX初期化
	dxCommon_ = DirectXCommon::GetInstance();

    // 入力システムの取得・初期化
    input_ = Input::GetInstance();

    // テクスチャ管理・スプライト共通設定の初期化
    TextureManager::GetInstance()->Initialize(dxCommon_);
    spriteCommon_ = SpriteCommon::GetInstance();
    spriteCommon_->Initialize(dxCommon_);

    // モデル周りの初期化
    modelCommon_ = new ModelCommon();
    modelCommon_->Initialize(dxCommon_);

    // 3D 共通設定初期化
    object3dCommon_ = ObJect3dCommon::GetInstance();
    object3dCommon_->Initialize(dxCommon_);

    // モデルマネージャ初期化
    ModelManager::GetInstance()->Initialize(dxCommon_);
    
	//Audioの初期化
    audio_ = Audio::GetInstance();
    audio_->Initialize(); //XAudio2とmasterVoiceの初期化を確実に行う
    // カメラ / レールカメラの作成と初期配置
    railCamera_ = railCamera;
    // スプライト群
    title_ = new Sprite();
    title_->Initialize(spriteCommon_, "resources/title/title.png");
	gameStart_ = new Sprite();
	gameStart_->Initialize(spriteCommon_, "resources/title/gameStart.png");
	explanation_ = new Sprite();
	explanation_->Initialize(spriteCommon_, "resources/title/explanation.png");
	operation_ = new Sprite();
	operation_->Initialize(spriteCommon_, "resources/title/operation.png");
    backGround_ = new Sprite();
    backGround_->Initialize(spriteCommon_, "resources/backGround.png"); 
    backGround_->SetSize(Vector2(1280, 720));

	// 点滅初期設定: 初期色を通常にしておく
	gameStart_->SetColor(normalColor_);
	explanation_->SetColor(normalColor_);
	blinkTimer_ = 0;
	blinkState_ = false;
	goToGame = false;

	// 操作説明
    isExplanation_ = false;
}

void TitleScene::Update()
{
    title_->Update();
	gameStart_->Update();
	explanation_->Update();
    operation_->Update();
	backGround_->Update();
	audio_->Update();

	// 点滅処理
	++blinkTimer_;
	if (blinkTimer_ >= blinkInterval_) {
		blinkTimer_ = 0;
		blinkState_ = !blinkState_;
		Vector4 c = blinkState_ ? blinkColor_ : normalColor_;
		if (gameStart_) gameStart_->SetColor(c);
		if (explanation_) explanation_->SetColor(c);
	}

	// 操作説明の表示切替:
	// キーボードの T キー、またはゲームパッドの L1(LEFT_SHOULDER) を押すとトグル表示
	if (input_) {
		bool toggleRequested = false;
		if (input_->TriggerKey(DIK_T)) {
			toggleRequested = true;
		}
		// ゲームパッド入力は GamepadButtonTrigger を想定
		if (input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
			toggleRequested = true;
		}

		if (toggleRequested) {
			isExplanation_ = !isExplanation_;
		}

		// 補足: 操作説明表示中にもう一度 T または L1 を押せば閉じる（上と同じトグル挙動）
	}

    if (input_->TriggerKey(DIK_RETURN) || input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_A)) {
		goToGame = true;
    }
}

void TitleScene::Draw()
{
    // タイトルメイン画像
	spriteCommon_->SettingCommonDraw();
    backGround_->Draw();
    title_->Draw();
	gameStart_->Draw();
	explanation_->Draw();
    if (isExplanation_) {
        operation_->Draw();
    }
}

void TitleScene::Finalize()
{
    delete title_; 
    title_ = nullptr;
	delete gameStart_; 
    gameStart_ = nullptr;
	delete explanation_;
	explanation_ = nullptr;
	delete operation_;
	operation_ = nullptr;
    delete modelCommon_; 
    modelCommon_ = nullptr;
    // railCameraは所有しないのでdeleteしない
    railCamera_ = nullptr;
}