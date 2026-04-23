#include "StringUility.h"
#include <d3d12.h>


#pragma comment (lib, "d3d12.lib")

namespace StringUtility {
	// UTF-8のstd::stringをUTF-16のstd::wstringに変換するユーティリティ。
	// MultiByteToWideCharで必要サイズを取得してからバッファへ変換する。
	// 注意:
	//   MultiByteToWideCharの返すサイズと終端文字の扱いに注意。
	//   入力が空のときは空のwstringを返す。
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

	// UTF-16をUTF-8のstd::stringに変換するユーティリティ。
	// WideCharToMultiByteで必要サイズを取得してからバッファへ変換する。
	// 注意:
	//   WideCharToMultiByte の返すサイズと終端文字の扱いに注意。
	//   入力が空のときは空の string を返す。
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