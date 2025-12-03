#pragma once
#include <string>

/// <summary>
/// ログ出力ユーティリティ（名前空間関数）.
/// 
/// 用途:
/// - デバッグ情報やエラーメッセージを出力するための簡易ラッパー。
/// - 実装では OutputDebugString やファイル出力などに振り分ける想定。
/// 
/// 使用例:
/// - Logger::Log("初期化に失敗しました");
/// 
/// 備考:
/// - マルチスレッドから使う場合は実装側で同期を行ってください。
/// </summary>

namespace Logger {
	// メッセージをログ出力する
	void Log(const std::string& message);
}