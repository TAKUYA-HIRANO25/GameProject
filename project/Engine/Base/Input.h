/// <summary>
/// 入力管理クラス
/// 
/// 概要:
/// - キーボードとマウスの状態を DirectInput を用いて取得・管理するユーティリティクラス。
/// - 現在フレーム／前フレームの入力状態を保持し、押下（Push）・トリガー（Trigger）判定や
///   マウスの相対移動・ホイール・クライアント座標の取得を提供する。
/// 
/// 主な責務:
/// - `Initialize(WinApp*)` で DirectInput とデバイスの初期化を行う。
/// - `Update()` でデバイスの状態を読み取り、内部キャッシュを更新する。
/// - `PushKey` / `TriggerKey` でキーボード入力判定を行う。
/// - `PushMouse` / `TriggerMouse`、`GetMouseMoveX/Y`、`GetMouseWheel` でマウス関連情報を取得する。
/// - `GetCursorClientPos2` / `GetCursorClientPos3` でクライアント座標系のカーソル位置（2D/3D）を取得する（必要に応じて変換して使用）。
/// 
/// 注意:
/// - DirectInput の初期化と更新は通常メインスレッドで行うこと。スレッドセーフではない。
/// - ウィンドウハンドルは `WinApp` 経由で提供される想定。`Initialize` に渡す `WinApp*` は有効である必要がある。
/// - マウスの相対移動はフレームごとの差分を返すため、描画ループ内で毎フレーム `Update` を呼び出すこと。
/// </summary>

#pragma once

#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "WinApp.h"
#include "Vector2.h"
#include "Vector3.h"

class Input {

public:
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	// シングルトン取得（Meyers）
	static Input* GetInstance();

	// 初期化
	void Initialize(WinApp* winApp);
	// 更新
	void Update();

	// --- キーボード関連 ---
	bool PushKey(BYTE keyNumber);
	// 押した瞬間
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

	// --- ゲームパッド関連（追加API） ---
	// フレームごとにポーリングする（Input::Update 内で呼ぶか、外部から毎フレーム呼ぶ）
	void UpdateGamepads();

	// コントローラ接続確認（index: 0..3）
	bool IsGamepadConnected(uint32_t index) const noexcept;

	// ボタン押下 / トリガー（index: 0..3, button: XINPUT_GAMEPAD_...）
	bool GamepadButtonPush(uint32_t index, WORD button) const noexcept;
	bool GamepadButtonTrigger(uint32_t index, WORD button) const noexcept;

	// 軸値（-1.0 .. 1.0） / トリガー（0.0 .. 1.0）
	float GetLeftThumbX(uint32_t index) const noexcept;
	float GetLeftThumbY(uint32_t index) const noexcept;
	float GetRightThumbX(uint32_t index) const noexcept;
	float GetRightThumbY(uint32_t index) const noexcept;
	float GetLeftTrigger(uint32_t index) const noexcept;
	float GetRightTrigger(uint32_t index) const noexcept;

private:
	// プライベート化（シングルトン）
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(Input&&) = delete;

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

	// ゲームパッド状態（最大4台）
	XINPUT_STATE gamepadState[4]{};
	XINPUT_STATE gamepadStatePrev[4]{};
	bool gamepadConnected[4]{};

	static Input* instance;
};