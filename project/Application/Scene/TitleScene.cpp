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
    audio_->LoadMP3("resources/Suond/GameBGM.mp3", "BGM");
    //audio_->Play("BGM", true);
    //audio_->SetMasterVolume(0.5f);
    // カメラ / レールカメラの作成と初期配置
    railCamera_ = railCamera;
    // スプライト群
    title_ = new Sprite();
    title_->Initialize(spriteCommon_, "resources/title.png");
    titleUI_ = new Sprite();
    titleUI_->Initialize(spriteCommon_, "resources/titleUI.png");
    backGround_ = new Sprite();
    backGround_->Initialize(spriteCommon_, "resources/backGround.png"); 
    backGround_->SetSize(Vector2(1280, 720));

	goToGame = false;
}

void TitleScene::Update()
{
    title_->Update();
	backGround_->Update();
	audio_->Update();
    if (input_->TriggerKey(DIK_RETURN) || input_->GamepadButtonPush(0, XINPUT_GAMEPAD_A)) {
		goToGame = true;
    }
}

void TitleScene::Draw()
{
    // タイトルメイン画像
	spriteCommon_->SettingCommonDraw();
    backGround_->Draw();
    title_->Draw();

}

void TitleScene::Finalize()
{
    delete title_; title_ = nullptr;
    delete titleUI_; titleUI_ = nullptr;
    delete modelCommon_; modelCommon_ = nullptr;
    // railCameraは所有しないのでdeleteしない
    railCamera_ = nullptr;
}