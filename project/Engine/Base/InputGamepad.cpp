#include "Input.h"
#include <algorithm>
#include <cmath>

namespace {
	constexpr int kMaxGamepads = 4;

	// スティック正規化ユーティリティ（デッドゾーン処理）
	inline float NormalizeThumb(short value, short deadZone) noexcept {
		if (std::abs(value) <= deadZone) return 0.0f;
		// 正規化（範囲 -32768..32767 -> -1..1）
		constexpr float norm = 1.0f / 32767.0f;
		return std::clamp(static_cast<float>(value) * norm, -1.0f, 1.0f);
	}

	inline float NormalizeTrigger(BYTE value, BYTE threshold) noexcept {
		if (value <= threshold) return 0.0f;
		return std::clamp(static_cast<float>(value) / 255.0f, 0.0f, 1.0f);
	}

	inline bool IsValidIndex(uint32_t index) noexcept {
		return index < kMaxGamepads;
	}
}

void Input::UpdateGamepads() {
	// 前フレームを保存
	for (uint32_t i = 0; i < kMaxGamepads; ++i) {
		gamepadStatePrev[i] = gamepadState[i];
	}

	// ポーリング
	for (DWORD i = 0; i < kMaxGamepads; ++i) {
		XINPUT_STATE state{};
		DWORD result = XInputGetState(i, &state);
		if (result == ERROR_SUCCESS) {
			gamepadConnected[i] = true;
			gamepadState[i] = state;
		} else {
			gamepadConnected[i] = false;
			// clear current state when disconnected to avoid stale bits
			gamepadState[i].dwPacketNumber = 0;
			gamepadState[i].Gamepad.wButtons = 0;
			gamepadState[i].Gamepad.bLeftTrigger = 0;
			gamepadState[i].Gamepad.bRightTrigger = 0;
			gamepadState[i].Gamepad.sThumbLX = 0;
			gamepadState[i].Gamepad.sThumbLY = 0;
			gamepadState[i].Gamepad.sThumbRX = 0;
			gamepadState[i].Gamepad.sThumbRY = 0;
		}
	}
}

bool Input::IsGamepadConnected(uint32_t index) const noexcept {
	if (!IsValidIndex(index)) return false;
	return gamepadConnected[index];
}

bool Input::GamepadButtonPush(uint32_t index, WORD button) const noexcept {
	if (!IsValidIndex(index) || !gamepadConnected[index]) return false;
	return (gamepadState[index].Gamepad.wButtons & button) != 0;
}

bool Input::GamepadButtonTrigger(uint32_t index, WORD button) const noexcept {
	if (!IsValidIndex(index) || !gamepadConnected[index]) return false;
	WORD curr = gamepadState[index].Gamepad.wButtons;
	WORD prev = gamepadStatePrev[index].Gamepad.wButtons;
	return ((curr & button) != 0) && ((prev & button) == 0);
}

float Input::GetLeftThumbX(uint32_t index) const noexcept {
	if (!IsValidIndex(index) || !gamepadConnected[index]) return 0.0f;
	return NormalizeThumb(gamepadState[index].Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
}
float Input::GetLeftThumbY(uint32_t index) const noexcept {
	if (!IsValidIndex(index) || !gamepadConnected[index]) return 0.0f;
	return NormalizeThumb(gamepadState[index].Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
}
float Input::GetRightThumbX(uint32_t index) const noexcept {
	if (!IsValidIndex(index) || !gamepadConnected[index]) return 0.0f;
	return NormalizeThumb(gamepadState[index].Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}
float Input::GetRightThumbY(uint32_t index) const noexcept {
	if (!IsValidIndex(index) || !gamepadConnected[index]) return 0.0f;
	return NormalizeThumb(gamepadState[index].Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

float Input::GetLeftTrigger(uint32_t index) const noexcept {
	if (!IsValidIndex(index) || !gamepadConnected[index]) return 0.0f;
	return NormalizeTrigger(gamepadState[index].Gamepad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}
float Input::GetRightTrigger(uint32_t index) const noexcept {
	if (!IsValidIndex(index) || !gamepadConnected[index]) return 0.0f;
	return NormalizeTrigger(gamepadState[index].Gamepad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

// 追加: ゲームパッドが「使用中（アクティブ）」かどうかの判定
bool Input::IsGamepadActive(uint32_t index) const noexcept {
	if (!IsValidIndex(index)) return false;
	if (!gamepadConnected[index]) return false;

	const SHORT lx = gamepadState[index].Gamepad.sThumbLX;
	const SHORT ly = gamepadState[index].Gamepad.sThumbLY;
	const SHORT rx = gamepadState[index].Gamepad.sThumbRX;
	const SHORT ry = gamepadState[index].Gamepad.sThumbRY;
	const BYTE lt = gamepadState[index].Gamepad.bLeftTrigger;
	const BYTE rt = gamepadState[index].Gamepad.bRightTrigger;
	const WORD buttons = gamepadState[index].Gamepad.wButtons;

	// スティックがデッドゾーンを超えている
	if (std::abs(lx) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) return true;
	if (std::abs(ly) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) return true;
	if (std::abs(rx) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) return true;
	if (std::abs(ry) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) return true;

	// トリガが閾値を超えている
	if (lt > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) return true;
	if (rt > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) return true;

	// いずれかのボタンが押されている
	if (buttons != 0) return true;

	return false;
}

bool Input::IsAnyGamepadActive() const noexcept {
	for (uint32_t i = 0; i < 4; ++i) {
		if (IsGamepadActive(i)) return true;
	}
	return false;
}