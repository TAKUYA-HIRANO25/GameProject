#pragma once

/// <summary>
/// 2 次元ベクトル構造体
///
/// 用途:
/// - 2D 座標・UV などの表現に用いる軽量構造体。
/// - 演算は MyMath 名前空間や演算子オーバーロードで提供する想定。
/// </summary>
struct Vector2
{
	float x;
	float y;
};