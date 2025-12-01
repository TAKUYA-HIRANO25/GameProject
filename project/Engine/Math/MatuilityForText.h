#pragma once
#include "MyMath.h"


/// <summary>
/// ベクトルの演算子オーバーロード宣言
///
/// 概要:
/// - Vector3 に対する共通の算術演算子をグローバルに提供するヘッダ。
/// - +=, +, *=, *, -=, - を定義し、可読性の高いベクトル演算を可能にする。
///
/// 注意:
/// - 実体は対応する .cpp に実装すること（ヘッダのみの宣言）。
/// - 引数は参照や値渡しで適切に扱い、パフォーマンスと安全性を両立する。
/// </summary>

// += 演算子
Vector3& operator+=(Vector3& lhv, const Vector3& rhv);

// + 演算子
const Vector3 operator+(const Vector3& v1, const Vector3& v2);

// *= 演算子
Vector3& operator*=(Vector3& v, float s);

// * 演算子
const Vector3 operator*(const Vector3& v, float s);

// -= 演算子
Vector3& operator-=(Vector3& lhv, const Vector3& rhv);

// - 演算子
const Vector3 operator-(const Vector3& v1, const Vector3& v2);