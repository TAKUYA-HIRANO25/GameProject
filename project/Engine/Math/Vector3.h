#pragma once

/// <summary>
/// 3 次元ベクトル構造体
///
/// 用途:
/// - 位置・速度・方向ベクトルなど 3D 空間の基本データを表す。
/// - 長さや正規化などの演算は MyMath::Length / MyMath::Normalize などを利用すること。
/// </summary>
struct Vector3
{
	float x;
	float y;
	float z;
};