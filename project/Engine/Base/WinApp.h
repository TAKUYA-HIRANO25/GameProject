#pragma once
#include "Windows.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#include <cstdint>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

/// <summary>
/// Win32ウィンドウとメッセージループを管理するユーティリティ.
/// 
/// 責務:
/// - アプリケーションウィンドウの作成と破棄を行う。
/// - メッセージ取得と処理のラッパーを提供。
/// - ウィンドウプロシージャへの静的アクセスを提供。
/// 
/// インターフェース:
/// - Initialize__:ウィンドウを作成して初期化。
/// - Finalize__:ウィンドウと関連リソースを解放。
/// - ProcessMessage__:メッセージループを進め、ウィンドウが閉じられたかを判定。
/// 
/// 備考:
/// - クライアント領域のサイズは静的定数で定義されており、レンダリングの基準解像度として使用される。
/// - スレッドセーフではない(UIスレッドでのみ使用する前提)。
/// </summary>
class WinApp {
public:
	// シングルトン取得
	static WinApp* GetInstance();

	// 初期化
	void Initialize();

	// 終了
	void Finalize();

	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// ウィンドウハンドル取得
	HWND GetHwnd() const { return hwnd; }

	// インスタンスハンドル取得
	HINSTANCE GetWCInStance() const { return wc.hInstance; }

	// メッセージ処理
	bool ProcessMessage();

public:

	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;


private:

	HWND hwnd = nullptr;

	WNDCLASS wc{};

	// シングルトン
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	WinApp& operator=(const WinApp&) = delete;
	WinApp(WinApp&&) = delete;
	WinApp& operator=(WinApp&&) = delete;

	static WinApp* instance;
};