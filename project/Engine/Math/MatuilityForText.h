#pragma once
#include "MyMath.h"

Vector3& operator+=(Vector3& lhv, const Vector3& rhv);

const Vector3 operator+(const Vector3& v1, const Vector3& v2);

Vector3& operator*=(Vector3& v, float s);

const Vector3 operator*(const Vector3& v, float s);

Vector3& operator-=(Vector3& lhv, const Vector3& rhv);

const Vector3 operator-(const Vector3& v1, const Vector3& v2);