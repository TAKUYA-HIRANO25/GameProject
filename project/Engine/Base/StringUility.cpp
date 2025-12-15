#include "StringUility.h"
#include <d3d12.h>

#pragma comment (lib, "d3d12.lib")

namespace StringUtility {
	// ConvertString(std::string -> std::wstring)
	// - UTF-8 の std::string をワイド文字列 (UTF-16) に変換するユーティリティ。
	// - MultiByteToWideChar の呼び出しで必要サイズを取得してからバッファを確保して再度呼ぶ。
	// - 注意:
	//   - MultiByteToWideChar は要求されるワイド文字数を返しますが、API の使用方法によっては
	//     終端 '\0' を含むかどうかが異なります。ここでは返されたサイズで wstring を構築し、
	//     直接書き込んでいますが、末尾 NUL の扱いに注意してください。
	//   - 入力文字列が空のときは空の wstring を返します。
	std::wstring ConvertString(const std::string& str) {
		if (str.empty()) {
			return std::wstring();
		}

		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		if (sizeNeeded == 0) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
		return result;
	}

	// ConvertString(std::wstring -> std::string)
	// - UTF-16(wstring) を UTF-8 の std::string に変換するユーティリティ。
	// - WideCharToMultiByte で必要サイズを取得してからバッファへ変換する。
	// - 注意:
	//   - WideCharToMultiByte の返すサイズと終端文字の扱いに注意（必要なら +1 を行って NUL を明示的に付与する）。
	//   - 入力が空のときは空の string を返す。
	std::string ConvertString(const std::wstring& str)
	{
		if (str.empty()) {
			return std::string();
		}

		auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
		if (sizeNeeded == 0) {
			return std::string();
		}
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
		return result;
	}
}