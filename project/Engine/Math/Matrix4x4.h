#pragma once

/// <summary>
/// 4x4 行列構造体
///
/// 用途:
/// - 3D空間のアフィン変換や射影変換を扱うための基本行列型。
/// - 行列はm[row][col]の形式で保持。
///
/// 注意:
/// - 行列演算はMyMath名前空間の関数を利用してください。
/// </summary>
struct Matrix4x4
{
	float m[4][4];
};
