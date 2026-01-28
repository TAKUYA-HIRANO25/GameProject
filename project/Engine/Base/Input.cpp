#include "Input.h"
#include <cassert>
#include <algorithm>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

Input* Input::instance = nullptr;

Input* Input::GetInstance()
{
	if (instance == nullptr) {
		instance = new Input;
	}
	return instance;
}

void Input::Initialize(WinApp* winApp)
{
	HRESULT result;

	this->winApp = winApp;

	result = DirectInput8Create(winApp->GetWCInStance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//キーボードデバイス生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// --- マウスデバイス初期化 ---
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));
	result = mouse->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));
	// マウスは非排他モードでフォアグラウンド（他アプリと共存）
	result = mouse->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
	// 初期状態をクリア
	ZeroMemory(&mouseState, sizeof(mouseState));
	ZeroMemory(&mouseStatePrev, sizeof(mouseStatePrev));
	mouseMoveX = mouseMoveY = mouseWheel = 0;
}

void Input::Update()
{
	// キーボード前回保存
	memcpy(keyPre, key, sizeof(key));

	if (keyboard)
	{
		HRESULT hr = keyboard->Acquire();
		if (FAILED(hr)) {
			char buf[128];
			sprintf_s(buf, "Keyboard Acquire failed: 0x%08X\n", static_cast<unsigned>(hr));
			OutputDebugStringA(buf);
		}
		hr = keyboard->GetDeviceState(sizeof(key), key);
		if (FAILED(hr))
		{
			ZeroMemory(key, sizeof(key));
			char buf[128];
			sprintf_s(buf, "Keyboard GetDeviceState failed: 0x%08X\n", static_cast<unsigned>(hr));
			OutputDebugStringA(buf);
		}
	}

	if (mouse)
	{
		mouseStatePrev = mouseState;
		HRESULT hr = mouse->Acquire();
		if (FAILED(hr)) {
			char buf[128];
			sprintf_s(buf, "Mouse Acquire failed: 0x%08X\n", static_cast<unsigned>(hr));
			OutputDebugStringA(buf);
		}
		hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
		if (FAILED(hr))
		{
			mouseState.lX = mouseState.lY = mouseState.lZ = 0;
			mouseState.rgbButtons[0] = mouseState.rgbButtons[1] = mouseState.rgbButtons[2] = 0;
			char buf[128];
			sprintf_s(buf, "Mouse GetDeviceState failed: 0x%08X\n", static_cast<unsigned>(hr));
			OutputDebugStringA(buf);
		}
		mouseMoveX = mouseState.lX;
		mouseMoveY = mouseState.lY;
		mouseWheel = mouseState.lZ;
	}

	// ゲームパッドの状態を更新
	UpdateGamepads();
}

// キーボードロック用 API (Poose)
void Input::SetKeyboardLockedByPoose(bool locked) noexcept {
	keyboardLockedByPoose_ = locked;
}
bool Input::IsKeyboardLockedByPoose() const noexcept {
	return keyboardLockedByPoose_;
}

// マウスロック用 API (Poose)
void Input::SetMouseLockedByPoose(bool locked) noexcept {
	mouseLockedByPoose_ = locked;
}
bool Input::IsMouseLockedByPoose() const noexcept {
	return mouseLockedByPoose_;
}

// キーボードロック用 API (Controller)
void Input::SetKeyboardLockedByController(bool locked) noexcept {
	keyboardLockedByController_ = locked;
}
bool Input::IsKeyboardLockedByController() const noexcept {
	return keyboardLockedByController_;
}

// マウスロック用 API (Controller)
void Input::SetMouseLockedByController(bool locked) noexcept {
	mouseLockedByController_ = locked;
}
bool Input::IsMouseLockedByController() const noexcept {
	return mouseLockedByController_;
}

bool Input::PushKey(BYTE keyNumber) {
	// Poose または Controller によるロック中は常に false を返す（キーボード無効化）
	if (keyboardLockedByPoose_ || keyboardLockedByController_) return false;

	//指定キーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {
	// Poose または Controller によるロック中は常に false を返す（キーボード無効化）
	if (keyboardLockedByPoose_ || keyboardLockedByController_) return false;

	if (keyPre[keyNumber] == 0 && key[keyNumber] != 0) {
		return true;
	}
	return false;
}

// --- マウス用の実装 ---
// ボタン: 0 = 左, 1 = 右, 2 = 中
bool Input::PushMouse(uint8_t button) const noexcept
{
	// Poose または Controller によるロック中は常に false を返す（マウス無効化）
	if (mouseLockedByPoose_ || mouseLockedByController_) return false;

	if (button >= 8) return false;
	return (mouseState.rgbButtons[button] & 0x80) != 0;
}

bool Input::TriggerMouse(uint8_t button) const noexcept
{
	// Poose または Controller によるロック中は常に false を返す（マウス無効化）
	if (mouseLockedByPoose_ || mouseLockedByController_) return false;

	if (button >= 8) return false;
	return ((mouseStatePrev.rgbButtons[button] & 0x80) == 0) && ((mouseState.rgbButtons[button] & 0x80) != 0);
}

Vector2 Input::GetCursorClientPos2()
{
	// Poose または Controller によるマウスロック中は中央位置を返す（ポーズ中にマウスで操作されないようにする）
	if (mouseLockedByPoose_ || mouseLockedByController_) {
		return Vector2{ static_cast<float>(WinApp::kClientWidth) * 0.5f, static_cast<float>(WinApp::kClientHeight) * 0.5f };
	}

	// WinApp がセットされていない場合は (0,0) を返す
	if (!winApp) {
		return Vector2{ 0.0f, 0.0f };
	}

	POINT pt{};
	// スクリーン座標でカーソル位置を取得
	if (!GetCursorPos(&pt)) {
		return Vector2{ 0.0f, 0.0f };
	}

	HWND hwnd = winApp->GetHwnd();
	if (!hwnd) {
		return Vector2{ 0.0f, 0.0f };
	}

	// スクリーン座標 -> クライアント座標に変換
	ScreenToClient(hwnd, &pt);

	// クライアント矩形を取得（ウィンドウの実際のサイズに基づく）
	RECT rc{};
	if (!GetClientRect(hwnd, &rc)) {
		// 取得できなければ静的定数をフォールバック
		rc.left = 0;
		rc.top = 0;
		rc.right = WinApp::kClientWidth;
		rc.bottom = WinApp::kClientHeight;
	}

	// クライアント領域内にクランプ（境界は inclusive -> exclusive）
	int cx = std::clamp(pt.x, rc.left, rc.right - 1);
	int cy = std::clamp(pt.y, rc.top, rc.bottom - 1);

	return Vector2{ static_cast<float>(cx), static_cast<float>(cy) };
}

Vector3 Input::GetCursorClientPos3()
{
	Vector2 p2 = GetCursorClientPos2();
	// Z は用途に応じて設定してください（ここでは 0）
	return Vector3{ p2.x, p2.y, 0.0f };
}
