/// <summary>
/// 入力管理クラス
/// 
/// 概要:
/// - キーボードとマウスの状態を DirectInput を用いて取得・管理するユーティリティクラス。
/// - 現在フレーム／前フレームの入力状態を保持し、押下・トリガー判定や
///   マウスの相対移動・ホイール・クライアント座標の取得を提供する。
/// 
/// 主な責務:
/// - InitializeでDirectInputとデバイスの初期化を行う。
/// - Update() でデバイスの状態を読み取り、内部キャッシュを更新する。
/// - PushKey/TriggerKey` でキーボード入力判定を行う。
/// - PushMouse/TriggerMouse、GetMouseMoveX/Y、GetMouseWheelでマウス関連情報を取得する。
/// - GetCursorClientPos2/GetCursorClientPos3でクライアント座標系のカーソル位置を取得する。
/// 
/// 注意:
/// - DirectInput の初期化と更新は通常メインスレッドで行うこと。スレッドセーフではない。
/// - ウィンドウハンドルはWinApp経由で提供される想定。Initialize に渡すWinAppは有効である必要がある。
/// - マウスの相対移動はフレームごとの差分を返すため、描画ループ内で毎フレーム Updateを呼び出すこと。
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

	// シングルトン取得
	static Input* GetInstance();

	// 初期化
	void Initialize(WinApp* winApp);
	// 更新
	void Update();

	// キーボード関連
	bool PushKey(BYTE keyNumber);
	// 押した瞬間
	bool TriggerKey(BYTE keyNumber);

	// マウス関連
	// ボタン: 0 = 左, 1 = 右, 2 = 中
	bool PushMouse(uint8_t button) const noexcept;
	bool TriggerMouse(uint8_t button) const noexcept;

	// 相対移動
	int GetMouseMoveX() const noexcept { return mouseMoveX; }
	int GetMouseMoveY() const noexcept { return mouseMoveY; }

	// ホイール
	int GetMouseWheel() const noexcept { return mouseWheel; }

	// クライアント座標でのカーソル位置
	Vector2 GetCursorClientPos2();
	Vector3 GetCursorClientPos3();

	// ゲームパッド関連
	void UpdateGamepads();
	bool IsGamepadConnected(uint32_t index) const noexcept;
	bool GamepadButtonPush(uint32_t index, WORD button) const noexcept;
	bool GamepadButtonTrigger(uint32_t index, WORD button) const noexcept;
	float GetLeftThumbX(uint32_t index) const noexcept;
	float GetLeftThumbY(uint32_t index) const noexcept;
	float GetRightThumbX(uint32_t index) const noexcept;
	float GetRightThumbY(uint32_t index) const noexcept;
	float GetLeftTrigger(uint32_t index) const noexcept;
	float GetRightTrigger(uint32_t index) const noexcept;
	bool IsGamepadActive(uint32_t index) const noexcept;
	bool IsAnyGamepadActive() const noexcept;

	// Poose 等から入力ロックを要求するためのAPI
	void SetKeyboardLockedByPoose(bool locked) noexcept;
	bool IsKeyboardLockedByPoose() const noexcept;
	void SetMouseLockedByPoose(bool locked) noexcept;
	bool IsMouseLockedByPoose() const noexcept;

	// プレイヤー等が検出したコントローラによる入力ロック
	void SetKeyboardLockedByController(bool locked) noexcept;
	bool IsKeyboardLockedByController() const noexcept;
	void SetMouseLockedByController(bool locked) noexcept;
	bool IsMouseLockedByController() const noexcept;

private:
	// シングルトン
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

	// Poose による入力ロックフラグ
	bool keyboardLockedByPoose_ = false;
	bool mouseLockedByPoose_ = false;

	// コントローラ検出による入力ロックフラグ(プレイヤー側で使用)
	bool keyboardLockedByController_ = false;
	bool mouseLockedByController_ = false;

	static Input* instance;
};