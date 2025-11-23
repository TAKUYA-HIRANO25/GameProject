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
/// Win32 ウィンドウとメッセージループを管理するユーティリティ.
/// 
/// 責務:
/// - アプリケーションウィンドウの作成と破棄を行う。
/// - メッセージ取得と処理のラッパーを提供する（__ProcessMessage__）。
/// - ウィンドウプロシージャへの静的アクセスを提供する（__WindowProc__）。
/// 
/// インターフェース:
/// - __Initialize__: ウィンドウを作成して初期化する。
/// - __Finalize__: ウィンドウと関連リソースを解放する。
/// - __ProcessMessage__: メッセージループを進め、ウィンドウが閉じられたかを判定する。
/// 
/// 備考:
/// - クライアント領域のサイズは静的定数で定義されており、レンダリングの基準解像度として使用される。
/// - スレッドセーフではない（UI スレッドでのみ使用する前提）。
/// </summary>
class WinApp {
public:
	void Initialize();

	void Finalize();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND GetHwnd() const { return hwnd; }

	HINSTANCE GetWCInStance() const { return wc.hInstance; }

	bool ProcessMessage();

public:

	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;


private:

	HWND hwnd = nullptr;

	WNDCLASS wc{};

};