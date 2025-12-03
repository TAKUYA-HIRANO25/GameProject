#pragma once
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"


/// <summary>
/// ゲーム用数学ユーティリティ名前空間 (MyMath)
///
/// 概要:
/// - 行列・ベクトルの生成や変換、行列演算、正規化など、ゲームで頻繁に使用する数学関数を提供する。
/// - MakeAffineMatrix / MakePerspectiveFovMatrix / Inverse など、3D レンダリング向けの関数群を含む。
///
/// 注意:
/// - 関数の多くは行列やベクトルの乗算・逆行列計算を行うため、数値安定性に注意して使用すること。
/// - パフォーマンスが重要な箇所では呼び出し回数やコピー回数を最適化してください。
/// </summary>
namespace MyMath {
	
	//変換行列
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	// 平行光源
	struct DirectionalLight {
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

	//変換情報
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	//単位行列
	Matrix4x4 MakeIdentity4x4();
	//積
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	//スカラ
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);
	//移動
	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
	// x軸
	Matrix4x4 MakeRotateXMatrix(float radian);
	// Y軸
	Matrix4x4 MakeRotateYMatrix(float radian);
	// Z軸
	Matrix4x4 MakeRotateZMatrix(float radian);
	//座標変換
	Vector3 TransformS(const Vector3& vector, const Matrix4x4& matrix);
	//SRTの合成
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	//透視投影
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspecRatio, float nearClip, float farClip);
	//ビューポート
	Matrix4x4 MakeViewportmatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
	//逆行列
	Matrix4x4 Inverse(const Matrix4x4& m);
	//平行投影
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	//法線変換
	static Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);
	//ノーマライズ
	float Length(const Vector3& v);
	Vector3 Normalize(const Vector3& v);
}