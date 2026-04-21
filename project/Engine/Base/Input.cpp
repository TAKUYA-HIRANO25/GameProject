#include "Input.h"
#include <cassert>
#include <algorithm>
#include <windows.h>
#include <dinput.h>

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

	// DirectInput の hInstance はアプリのモジュールハンドルを使うのが安全
	HINSTANCE hinst = GetModuleHandle(nullptr);
	result = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	if (FAILED(result)) {
		char buf[128];
		sprintf_s(buf, "DirectInput8Create failed: 0x%08X\n", static_cast<unsigned>(result));
		OutputDebugStringA(buf);
		directInput = nullptr;
		return;
	}

	// キーボードデバイス生成（失敗してもアプリ継続）
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	if (FAILED(result)) {
		char buf[128];
		sprintf_s(buf, "CreateDevice(GUID_SysKeyboard) failed: 0x%08X\n", static_cast<unsigned>(result));
		OutputDebugStringA(buf);
		keyboard = nullptr;
	} else {
		// 入力データ形式のセット
		result = keyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(result)) {
			char buf[128];
			sprintf_s(buf, "SetDataFormat(keyboard) failed: 0x%08X\n", static_cast<unsigned>(result));
			OutputDebugStringA(buf);
			keyboard->Release();
			keyboard = nullptr;
		} else {
			// 排他制御レベルのセット（非排他/フォアグラウンド）
			result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
			if (FAILED(result)) {
				char buf[128];
				sprintf_s(buf, "SetCooperativeLevel(keyboard) failed: 0x%08X\n", static_cast<unsigned>(result));
				OutputDebugStringA(buf);
				// 継続はするがキーボードは利用できない可能性あり
			}
		}
	}

	// マウスデバイス初期化（同様に失敗を厳罰扱いしない）
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	if (FAILED(result)) {
		char buf[128];
		sprintf_s(buf, "CreateDevice(GUID_SysMouse) failed: 0x%08X\n", static_cast<unsigned>(result));
		OutputDebugStringA(buf);
		mouse = nullptr;
	} else {
		result = mouse->SetDataFormat(&c_dfDIMouse);
		if (FAILED(result)) {
			char buf[128];
			sprintf_s(buf, "SetDataFormat(mouse) failed: 0x%08X\n", static_cast<unsigned>(result));
			OutputDebugStringA(buf);
			mouse->Release();
			mouse = nullptr;
		} else {
			result = mouse->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			if (FAILED(result)) {
				char buf[128];
				sprintf_s(buf, "SetCooperativeLevel(mouse) failed: 0x%08X\n", static_cast<unsigned>(result));
				OutputDebugStringA(buf);
			}
		}
	}

	// 初期状態をクリア
	ZeroMemory(&mouseState, sizeof(mouseState));
	ZeroMemory(&mouseStatePrev, sizeof(mouseStatePrev));
	mouseMoveX = mouseMoveY = mouseWheel = 0;
}

void Input::Update()
{
	// キーボード前回保存
	memcpy(keyPre, key, sizeof(key));

	// キーボードはウィンドウがフォアグラウンドのときのみ Acquire / GetDeviceState を試す
	if (keyboard)
	{
		HWND fg = GetForegroundWindow();
		HWND myHwnd = winApp ? winApp->GetHwnd() : nullptr;

		if (myHwnd && fg == myHwnd) {
			HRESULT hr = keyboard->Acquire();
			if (FAILED(hr)) {
				// Acquire が失敗することは普通にある（別アプリが優先している等）
				char buf[128];
				sprintf_s(buf, "Keyboard Acquire failed: 0x%08X\n", static_cast<unsigned>(hr));
				OutputDebugStringA(buf);
				// 続けて GetDeviceState を呼ぶと DIERR_NOTACQUIRED 等で失敗するのでスキップしてキーをクリア
				ZeroMemory(key, sizeof(key));
			} else {
				hr = keyboard->GetDeviceState(sizeof(key), key);
				if (FAILED(hr))
				{
					// よくある失敗パターンに対してリトライを試みる
					if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
						keyboard->Acquire();
						hr = keyboard->GetDeviceState(sizeof(key), key);
					}
					if (FAILED(hr)) {
						ZeroMemory(key, sizeof(key));
						char buf[128];
						sprintf_s(buf, "Keyboard GetDeviceState failed: 0x%08X\n", static_cast<unsigned>(hr));
						OutputDebugStringA(buf);
					}
				}
			}
		} else {
			// フォアグラウンドでなければ Acquire を解除してキー状態はクリア
			keyboard->Unacquire();
			ZeroMemory(key, sizeof(key));
		}
	}

	if (mouse)
	{
		// マウスはウィンドウがフォアグラウンドの時のみ状態を取得
		HWND fg = GetForegroundWindow();
		HWND myHwnd = winApp ? winApp->GetHwnd() : nullptr;

		mouseStatePrev = mouseState;

		if (myHwnd && fg == myHwnd) {
			HRESULT hr = mouse->Acquire();
			if (FAILED(hr)) {
				char buf[128];
				sprintf_s(buf, "Mouse Acquire failed: 0x%08X\n", static_cast<unsigned>(hr));
				OutputDebugStringA(buf);
				// フォアグラウンドでも Acquire できない場合は状態をゼロクリアする
				mouseState.lX = mouseState.lY = mouseState.lZ = 0;
				mouseState.rgbButtons[0] = mouseState.rgbButtons[1] = mouseState.rgbButtons[2] = 0;
			} else {
				hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
				if (FAILED(hr))
				{
					// リトライ：Input lost / not acquired
					if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
						mouse->Acquire();
						hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
					}
					if (FAILED(hr))
					{
						mouseState.lX = mouseState.lY = mouseState.lZ = 0;
						mouseState.rgbButtons[0] = mouseState.rgbButtons[1] = mouseState.rgbButtons[2] = 0;
						char buf[128];
						sprintf_s(buf, "Mouse GetDeviceState failed: 0x%08X\n", static_cast<unsigned>(hr));
						OutputDebugStringA(buf);
					}
				}
			}
		} else {
			// フォアグラウンドでなければ Unacquire して状態をクリア
			mouse->Unacquire();
			mouseState.lX = mouseState.lY = mouseState.lZ = 0;
			mouseState.rgbButtons[0] = mouseState.rgbButtons[1] = mouseState.rgbButtons[2] = 0;
		}

		mouseMoveX = mouseState.lX;
		mouseMoveY = mouseState.lY;
		mouseWheel = mouseState.lZ;
	}

	// ゲームパッドの状態を更新
	UpdateGamepads();
}

// キーボードロック用API(Poose)
void Input::SetKeyboardLockedByPoose(bool locked) noexcept {
	keyboardLockedByPoose_ = locked;
}
bool Input::IsKeyboardLockedByPoose() const noexcept {
	return keyboardLockedByPoose_;
}

// マウスロック用API(Poose)
void Input::SetMouseLockedByPoose(bool locked) noexcept {
	mouseLockedByPoose_ = locked;
}
bool Input::IsMouseLockedByPoose() const noexcept {
	return mouseLockedByPoose_;
}

// キーボードロック用API(Controller)
void Input::SetKeyboardLockedByController(bool locked) noexcept {
	keyboardLockedByController_ = locked;
}
bool Input::IsKeyboardLockedByController() const noexcept {
	return keyboardLockedByController_;
}

// マウスロック用API(Controller)
void Input::SetMouseLockedByController(bool locked) noexcept {
	mouseLockedByController_ = locked;
}
bool Input::IsMouseLockedByController() const noexcept {
	return mouseLockedByController_;
}

bool Input::PushKey(BYTE keyNumber) {
	// PooseまたはControllerによるロック中は常にfalseを返す
	if (keyboardLockedByPoose_ || keyboardLockedByController_) return false;

	//指定キーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {
	// PooseまたはControllerによるロック中は常にfalseを返す
	if (keyboardLockedByPoose_ || keyboardLockedByController_) return false;

	if (keyPre[keyNumber] == 0 && key[keyNumber] != 0) {
		return true;
	}
	return false;
}

// マウス用の実装
// ボタン: 0 = 左, 1 = 右, 2 = 中
bool Input::PushMouse(uint8_t button) const noexcept
{
	// PooseまたはControllerによるロック中は常にfalseを返す
	if (mouseLockedByPoose_ || mouseLockedByController_) return false;

	if (button >= 8) return false;
	return (mouseState.rgbButtons[button] & 0x80) != 0;
}

bool Input::TriggerMouse(uint8_t button) const noexcept
{
	// PooseまたはControllerによるロック中は常にfalseを返す
	if (mouseLockedByPoose_ || mouseLockedByController_) return false;

	if (button >= 8) return false;
	return ((mouseStatePrev.rgbButtons[button] & 0x80) == 0) && ((mouseState.rgbButtons[button] & 0x80) != 0);
}

Vector2 Input::GetCursorClientPos2()
{
	// PooseまたはControllerによるマウスロック中は中央位置を返す
	if (mouseLockedByPoose_ || mouseLockedByController_) {
		return Vector2{ static_cast<float>(WinApp::kClientWidth) * 0.5f, static_cast<float>(WinApp::kClientHeight) * 0.5f };
	}

	// WinAppがセットされていない場合は (0,0) を返す
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

	// スクリーン座標->クライアント座標に変換
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

	// クライアント領域内にクランプ
	int cx = std::clamp(pt.x, rc.left, rc.right - 1);
	int cy = std::clamp(pt.y, rc.top, rc.bottom - 1);

	return Vector2{ static_cast<float>(cx), static_cast<float>(cy) };
}

Vector3 Input::GetCursorClientPos3()
{
	Vector2 p2 = GetCursorClientPos2();
	// Zは用途に応じて設定
	return Vector3{ p2.x, p2.y, 0.0f };
}
