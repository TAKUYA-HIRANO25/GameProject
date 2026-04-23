#pragma once
#include <string>

/// <summary>
/// 文字列変換ユーティリティ.
/// 
/// 機能:
/// UTF-8/ANSIのstd::stringとUTF-16のstd::wstring間の変換を提供。
/// Win32APIのワイド文字APIを使う箇所とstd::stringを受け渡す際に使用。
/// 
/// 注意:
/// 文字コードやロケールに依存する変換のため、必要に応じて実装を調整。
/// </summary>

namespace StringUtility {
	// std::string を std::wstring に変換
	std::wstring ConvertString(const std::string& str);

	// std::wstring を std::string に変換
	std::string ConvertString(const std::wstring& str);
}