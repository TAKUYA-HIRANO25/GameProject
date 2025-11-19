#pragma once
#include "Windows.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#include "WinApp.h"
#include "Vector2.h"
#include "Vector3.h"

/// <summary>
/// 入力管理クラス
/// </summary>

class Input {

public:
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	void Initialize(WinApp* winApp);

	void Update();

	bool PushKey(BYTE keyNumber);

	bool TriggerKey(BYTE keyNumber);

	// --- マウス関連 ---
	// ボタン: 0 = 左, 1 = 右, 2 = 中
	bool PushMouse(uint8_t button) const noexcept;
	bool TriggerMouse(uint8_t button) const noexcept;

	// 相対移動（フレームごとの delta）
	int GetMouseMoveX() const noexcept { return mouseMoveX; }
	int GetMouseMoveY() const noexcept { return mouseMoveY; }

	// ホイール（相対）
	int GetMouseWheel() const noexcept { return mouseWheel; }

	// クライアント座標でのカーソル位置（必要なら使用）
	Vector2 GetCursorClientPos2();

	Vector3 GetCursorClientPos3();
private:

	ComPtr<IDirectInputDevice8>keyboard;

	BYTE key[256] = {};

	BYTE keyPre[256] = {};

	ComPtr<IDirectInput8>directInput;

	WinApp* winApp = nullptr;

	// マウスデバイス
	ComPtr<IDirectInputDevice8> mouse;

	// 現在 / 前回の状態
	DIMOUSESTATE mouseState{};         // current
	DIMOUSESTATE mouseStatePrev{};     // previous

	// 便利なキャッシュ値
	int mouseMoveX = 0;
	int mouseMoveY = 0;
	int mouseWheel = 0;
};