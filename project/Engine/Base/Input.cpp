#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


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
	// --- キーボード ---
	// 前回状態を保存
	memcpy(keyPre, key, sizeof(key));
	//キーボード情報の取得開始
	if (keyboard)
	{
		HRESULT hr = keyboard->Acquire();
		if (FAILED(hr))
		{
			// Acquire に失敗しても継続（アプリのフォーカス外など）
		}
		// 一度に大量のエラーが出る場合もあるので戻り値はチェックするが assert しない
		hr = keyboard->GetDeviceState(sizeof(key), key);
		if (FAILED(hr))
		{
			// 取得失敗時はキー配列をゼロにしておく（安全策）
			ZeroMemory(key, sizeof(key));
		}
	}

	// --- マウス ---
	if (mouse)
	{
		// 前回状態を保存
		mouseStatePrev = mouseState;

		HRESULT hr = mouse->Acquire();
		if (FAILED(hr))
		{
			// Acquire に失敗しても継続
		}

		hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
		if (FAILED(hr))
		{
			// 取得失敗時は移動量をゼロクリア
			mouseState.lX = mouseState.lY = mouseState.lZ = 0;
			mouseState.rgbButtons[0] = mouseState.rgbButtons[1] = mouseState.rgbButtons[2] = 0;
		}

		// 相対移動・ホイールをキャッシュ
		mouseMoveX = mouseState.lX;
		mouseMoveY = mouseState.lY;
		mouseWheel = mouseState.lZ;
	}
}


bool Input::PushKey(BYTE keyNumber) {
	//指定キーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {
	if (keyPre[keyNumber] == 0 && key[keyNumber] != 0) {
		return true;
	}
	return false;
}


// --- マウス用の実装 ---
// ボタン: 0 = 左, 1 = 右, 2 = 中
bool Input::PushMouse(uint8_t button) const noexcept
{
	if (button >= 8) return false;
	return (mouseState.rgbButtons[button] & 0x80) != 0;
}

bool Input::TriggerMouse(uint8_t button) const noexcept
{
	if (button >= 8) return false;
	return ((mouseStatePrev.rgbButtons[button] & 0x80) == 0) && ((mouseState.rgbButtons[button] & 0x80) != 0);
}

Vector2 Input::GetCursorClientPos2()
{
	POINT pos;
	GetCursorPos(&pos);
	Vector2 result = { float(pos.x), float(pos.y) };
	return result;
}

Vector3 Input::GetCursorClientPos3()
{
	POINT pos;
	GetCursorPos(&pos);
	Vector3 result = { float(pos.x), float(pos.y), 0.0f };
	return result;
}
