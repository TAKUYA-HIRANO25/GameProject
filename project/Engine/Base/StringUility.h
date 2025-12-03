#pragma once
#include <string>

/// <summary>
/// 文字列変換ユーティリティ.
/// 
/// 機能:
/// - UTF-8 / ANSI の std::string と UTF-16 の std::wstring 間の変換を提供する。
/// - Win32 API のワイド文字 API を使う箇所と std::string を受け渡す際に使用する。
/// 
/// 備考:
/// - 文字コードやロケールに依存する変換のため、必要に応じて実装を調整してください。
/// </summary>

namespace StringUtility {
	// std::string を std::wstring に変換
	std::wstring ConvertString(const std::string& str);

	// std::wstring を std::string に変換
	std::string ConvertString(const std::wstring& str);
}