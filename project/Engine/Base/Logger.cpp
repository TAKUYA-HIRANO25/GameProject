#include "Logger.h"
#include <d3d12.h>

#pragma comment (lib, "d3d12.lib")

namespace Logger {
	// Log:
	// デバッグ出力にメッセージを書き出す軽量ロガー。
	// OutputDebugStringA はスレッドセーフに使用できるが、大量ログを出すと
	//   デバッガ接続時のパフォーマンスに影響する点に注意する。
	// 必要に応じてファイル出力やログレベル制御を追加して拡張してください。
	void Log(const std::string& message)
	{
		OutputDebugStringA(message.c_str());
	}
}