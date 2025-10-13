#pragma once
#include "MyMath.h"

MyMath::Vector3& operator+=(MyMath::Vector3& lhv, const MyMath::Vector3& rhv);

const MyMath::Vector3 operator+(const MyMath::Vector3& v1, const MyMath::Vector3& v2);

MyMath::Vector3& operator*=(MyMath::Vector3& v, float s);

const MyMath::Vector3 operator*(const MyMath::Vector3& v, float s);

MyMath::Vector3& operator-=(MyMath::Vector3& lhv, const MyMath::Vector3& rhv);

const MyMath::Vector3 operator-(const MyMath::Vector3& v1, const MyMath::Vector3& v2);