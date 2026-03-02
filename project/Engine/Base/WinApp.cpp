#include "WinApp.h"
#include <cassert>
#pragma comment(lib, "winmm.lib")

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif 

// シングルトン実体の定義
WinApp* WinApp::instance = nullptr;

WinApp* WinApp::GetInstance()
{
	if (instance == nullptr) {
		instance = new WinApp;
	}
	return instance;
}

// WindowProc:
// - アプリケーションのウィンドウプロシージャ。
// - ImGui を使用している場合は最初に ImGui のハンドラにメッセージを渡し、
//   処理済みであればここで終了する。
// - WM_DESTROY を受け取ったら PostQuitMessage を呼び、アプリケーション終了フラグを送出する。
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif 
	switch (msg)
	{

	case WM_DESTROY:

		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ProcessMessage:
// - 非ブロッキングでメッセージを取得してディスパッチする。
// - WM_QUIT が来ていれば true を返してアプリケーションループに終了指示を伝える。
// - 注意: PeekMessage によるポーリングはメインループに組み込まれているため、CPU 使用率に
//   配慮する場合はスリープやタイミング制御を検討する。
bool WinApp::ProcessMessage()
{
	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}

// Initialize:
// - ウィンドウクラスの登録とウィンドウ生成、表示を行う。
// - timeBeginPeriod(1) を呼んでタイマー精度を上げている（全体影響に注意）。
// - 注意: COM 初期化や WinAPI の前提が別箇所である場合はここで行うか呼び出し元で保証すること。
void WinApp::Initialize()
{

	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"CG2WindowClass";
	wc.hInstance = GetModuleHandle(nullptr);

	RegisterClass(&wc);

	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindow(
		wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	ShowWindow(hwnd, SW_SHOW);

	// 高精度タイマー要求（1ms 単位）
	timeBeginPeriod(1);

}

// Finalize:
// - ウィンドウを閉じ、COM 等の後片付けを行う（ここでは CoUninitialize を呼んでいる）。
// - 注意: CreateWindow で確保したリソースがある場合は適切に解放する。
void WinApp::Finalize()
{

	CloseWindow(hwnd);
	CoUninitialize();
}

