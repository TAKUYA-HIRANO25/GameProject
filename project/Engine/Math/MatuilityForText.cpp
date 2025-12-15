#include "MatuilityForText.h"
#include <cmath>
#include <numbers>
using namespace MyMath;

// Utility for Vector3 binary operators
// - このファイルは Vector3 に対する演算子オーバーロードをまとめたもの。
// - すべてインプレース（lhs を更新）と非インプレース（新しい値を返す）を提供する。
// - 前提: Vector3 の各要素は有限な float 値であること。
// - 注意: 演算によるオーバーフロー/アンダーフローは呼び出し側で考慮する必要がある。

// Vector3 の加算 (lhs に rhs を加算して lhs を返す)
// - 高頻度に呼ばれる可能性があるため参照で受け取り直接更新する実装。
Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {
	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;
	return lhv;
}

// 非破壊的な加算: 新しい Vector3 を返す
const Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp += v2;
}

// スカラー乗算の代入 (lhs を s 倍して返す)
Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

// 非破壊的なスカラー乗算: 新しい Vector3 を返す
const Vector3 operator*(const Vector3& v, float s) {
	Vector3 temp(v);
	return temp *= s;
}

// Vector3 の減算 (lhs から rhs を減算して lhs を返す)
Vector3& operator-=(Vector3& lhv, const Vector3& rhv) {
	lhv.x -= rhv.x;
	lhv.y -= rhv.y;
	lhv.z -= rhv.z;
	return lhv;
}

// 非破壊的な減算: 新しい Vector3 を返す
const Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp -= v2;
}

