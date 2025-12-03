#pragma once

/// <summary>
/// 4 次元ベクトル構造体
///
/// 用途:
/// - 同次座標（w 成分）や色表現 (RGBA) などに使用する。
/// - 同次座標を用いた変換や光源情報の格納に適している。
/// </summary>
struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};
