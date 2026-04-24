#include "Poose.h"
#include "TextureManager.h"

// 簡易レイアウト定数(プロジェクトの画面サイズに合わせる)
namespace {
	constexpr float kScreenW = 1280.0f;
	constexpr float kScreenH = 720.0f;
}

Poose::Poose() = default;
Poose::~Poose()
{
	delete overlaySprite_; overlaySprite_ = nullptr;
	delete resumeSprite_; resumeSprite_ = nullptr;
}

void Poose::Initialize()
{
	spriteCommon_ = spriteCommon_->GetInstance();
	input_ = Input::GetInstance();
	EnsureSpritesInitialized();
}

void Poose::EnsureSpritesInitialized()
{
	if (overlaySprite_) return;

	// 全画面の半透明オーバーレイ
	overlaySprite_ = new Sprite();
	overlaySprite_->Initialize(spriteCommon_, "resources/backGround.png");
	overlaySprite_->SetSize(Vector2(kScreenW, kScreenH));
	overlaySprite_->SetPosition(Vector2(kScreenW * 0.5f, kScreenH * 0.5f));
	overlaySprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));
	overlaySprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f)); // 半透明

	// 選択肢表示
	resumeSprite_ = new Sprite();
	resumeSprite_->Initialize(spriteCommon_, "resources/GameBack.png");

	// 選択中の色や通常色は UpdateSpritesで切り替えるので基本色は白
	resumeSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void Poose::Activate() noexcept
{
	active_ = true;
	result_ = Result::None;
	selectedIndex_ = 0;
	EnsureSpritesInitialized();

	// 既にコントローラが使用されている場合はキーボードとマウスをロックしておく
	if (input_ && input_->IsAnyGamepadActive()) {
		input_->SetKeyboardLockedByPoose(true);
		input_->SetMouseLockedByPoose(true);
	}
}

void Poose::Deactivate() noexcept
{
	// Poose を閉じるときはキーボードとマウスロックを解除
	if (input_) {
		input_->SetKeyboardLockedByPoose(false);
		input_->SetMouseLockedByPoose(false);
	}

	active_ = false;
	result_ = Result::None;
}

bool Poose::IsActive() const noexcept
{
	return active_;
}

void Poose::Update()
{
	UpdateInput();
	UpdateSprites();
}

void Poose::UpdateInput()
{
	if (!input_) return;

	// コントローラ使用をここで検出したらキーボードとマウスをロック(閉じるまで解除しない)
	if (input_->IsAnyGamepadActive()) {
		input_->SetKeyboardLockedByPoose(true);
		input_->SetMouseLockedByPoose(true);
	}

	// Rで復帰
	if (input_->TriggerKey(DIK_R)) {
		result_ = Result::Resume;
		return;
	}

	// 上下で選択 
	if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W)) {
		selectedIndex_ = 0;
	}
	else if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S)) {
		selectedIndex_ = 1;
	}

	// ゲームパッド
	if (input_->IsAnyGamepadActive()) {
		// D-Pad 上/下
		if (input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_DPAD_UP)) {
			selectedIndex_ = 0;
		}
		else if (input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_DPAD_DOWN)) {
			selectedIndex_ = 1;
		}

		// A ボタンで決定
		if (input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_A)) {
			result_ = (selectedIndex_ == 0) ? Result::Resume : Result::ToTitle;
			return;
		}

		// B ボタンはキャンセル（復帰）として扱う
		if (input_->GamepadButtonTrigger(0, XINPUT_GAMEPAD_B)) {
			result_ = Result::Resume;
			return;
		}
	}

	// 決定 (キーボード)
	if (input_->TriggerKey(DIK_RETURN) || input_->TriggerKey(DIK_SPACE)) {
		result_ = (selectedIndex_ == 0) ? Result::Resume : Result::ToTitle;
		return;
	}

	// ショートカット:Tでタイトルに戻る
	if (input_->TriggerKey(DIK_T)) {
		result_ = Result::ToTitle;
	}
}

void Poose::UpdateSprites()
{
	if (!overlaySprite_) return;

	overlaySprite_->Update();

	Vector4 normalColor{ 1.0f,1.0f,1.0f,1.0f };
	Vector4 selectedColor{ 1.0f,0.95f,0.7f,1.0f };

	if (resumeSprite_) {
		resumeSprite_->SetColor(selectedIndex_ == 0 ? selectedColor : normalColor);
		resumeSprite_->Update();
	}
}

void Poose::Draw()
{

	// 描画は呼び出し元がspriteCommon_->SettingCommonDrawを呼んでいる前提
	if (overlaySprite_) overlaySprite_->Draw();
	if (resumeSprite_) resumeSprite_->Draw();

}

Poose::Result Poose::GetResult() const noexcept
{
	return result_;
}

void Poose::ClearResult() noexcept
{
	result_ = Result::None;
}