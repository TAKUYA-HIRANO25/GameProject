#include "MyMath.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

namespace MyMath {
	// MakeIdentity4x4:
	// - 単位行列を生成して返すヘルパ。
	// - 注意: ファイル内に duplicate return 文があるが動作上は後続の return に到達しない（無害）。
	Matrix4x4 MyMath::MakeIdentity4x4()
	{
		Matrix4x4 result{ 0 };
		result.m[0][0] = 1;
		result.m[1][1] = 1;
		result.m[2][2] = 1;
		result.m[3][3] = 1;
		return result;
		return result;
	}

	// Multiply:
	// - 4x4 行列の掛け算。標準的な行列積を計算する。
	// - パフォーマンス要件がある場合はループ順やSIMD最適化を検討する。
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		Matrix4x4 result{};
		for (int I = 0; I < 4; I++) {
			for (int J = 0; J < 4; J++) {
				result.m[I][J] = m1.m[I][0] * m2.m[0][J] + m1.m[I][1] * m2.m[1][J] + m1.m[I][2] * m2.m[2][J] + m1.m[I][3] * m2.m[3][J];
			}
		}
		return result;
	}

	// MakeScaleMatrix / MakeTranslateMatrix / MakeRotateX/Y/Z:
	// - 基本的なアフィン変換行列を作成するユーティリティ群。
	// - 右手系 / 行列の配置（行優先 or 列優先）はプロジェクト全体の実装に依存するため、
	//   使用時は他の行列生成と整合すること。
	Matrix4x4 MakeScaleMatrix(const Vector3& scale)
	{
		Matrix4x4 result = { 0 };
		result.m[0][0] = scale.x;
		result.m[1][1] = scale.y;
		result.m[2][2] = scale.z;
		result.m[3][3] = 1;

		return result;
	}

	Matrix4x4 MakeTranslateMatrix(const Vector3& translate)
	{
		Matrix4x4 result = { 0 };
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j) {
					result.m[i][j] = 1;
				}
			}
		}
		result.m[3][0] = translate.x;
		result.m[3][1] = translate.y;
		result.m[3][2] = translate.z;
		return result;
	}

	Matrix4x4 MakeRotateXMatrix(float radian)
	{
		float cosTheta = std::cos(radian);
		float sinTheta = std::sin(radian);
		Matrix4x4 result{ 0 };
		result.m[0][0] = 1.0f;
		result.m[1][1] = cosTheta;
		result.m[1][2] = sinTheta;
		result.m[2][1] = -sinTheta;
		result.m[2][2] = cosTheta;
		result.m[3][3] = 1.0f;

		return result;
	}

	Matrix4x4 MakeRotateYMatrix(float radian)
	{
		float cosTheta = std::cos(radian);
		float sinTheta = std::sin(radian);
		Matrix4x4 result{ 0 };
		result.m[1][1] = 1.0f;
		result.m[0][0] = cosTheta;
		result.m[2][0] = sinTheta;
		result.m[0][2] = -sinTheta;
		result.m[2][2] = cosTheta;
		result.m[3][3] = 1.0f;

		return result;
	}

	Matrix4x4 MakeRotateZMatrix(float radian)
	{
		float cosTheta = std::cos(radian);
		float sinTheta = std::sin(radian);
		Matrix4x4 result{ 0 };
		result.m[2][2] = 1.0f;
		result.m[0][0] = cosTheta;
		result.m[0][1] = sinTheta;
		result.m[1][0] = -sinTheta;
		result.m[1][1] = cosTheta;
		result.m[3][3] = 1.0f;

		return result;
	}

	// TransformS:
	// - 同次座標を含む変換を行い、w で正規化して結果を返す。
	// - 注意: 現在の実装は w が 0 のときのガードが空になっているため、呼び出し元は
	//         行列が w=0 を作らないことを保証するか、ここでのエラーハンドリングを追加すること。
	Vector3 TransformS(const Vector3& vector, const Matrix4x4& matrix)
	{
		Vector3 result = { 0 };
		result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
		result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
		result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
		float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
		if (w != 0) {
			// 本来ならここで早期 return か何らかの処理を行うが、現在は空ブロックになっている。
		}
		// w が 0 の場合は除算で NaN/Inf を生じる可能性があるため注意。
		result.x /= w;
		result.y /= w;
		result.z /= w;
		return result;
	}

	// MakeAffineMatrix:
	// - scale -> rotateX -> rotateY -> rotateZ -> translate の順で合成したアフィン行列を返す。
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
	{
		Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
		Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
		Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
		Matrix4x4 rotateXYZMatrix = Multiply(Multiply(rotateXMatrix, rotateYMatrix), rotateZMatrix);
		return Multiply(Multiply(MakeScaleMatrix(scale), rotateXYZMatrix), MakeTranslateMatrix(translate));
	}

	// MakePerspectiveFovMatrix:
	// - 透視投影行列を作成する。fovY はラジアン単位、near/far の扱いは呼び出し側と整合させること。
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspecRatio, float nearClip, float farClip)
	{
		Matrix4x4 result{ 0 };

		float cot = 1 / std::tan(fovY / 2.0f);
		result.m[0][0] = (1 / aspecRatio) * cot;
		result.m[1][1] = cot;
		result.m[2][2] = farClip / (farClip - nearClip);
		result.m[2][3] = 1;
		result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
		return result;
	}

	// MakeViewportmatrix:
	// - ビューポート変換行列（スクリーン座標変換）を作成する。
	Matrix4x4 MakeViewportmatrix(float left, float top, float width, float height, float minDepth, float maxDepth)
	{
		Matrix4x4 result{ 0 };
		result.m[0][0] = width / 2.0f;
		result.m[1][1] = -height / 2.0f;
		result.m[2][2] = maxDepth - minDepth;
		result.m[3][0] = left + width / 2.0f;
		result.m[3][1] = top + height / 2.0f;
		result.m[3][2] = minDepth;
		result.m[3][3] = 1.0f;

		return result;
	}

	// Inverse:
	// - 4x4 行列の逆行列を計算して返す。
	// - 行列が特異（determinant == 0）の場合は NaN/Inf を返す可能性があるため、
	//   呼び出し側で事前に行列の可逆性を確認するか、ここでの防御処理を追加すること。
	Matrix4x4 Inverse(const Matrix4x4& m)
	{
		float determinant =
			+m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
			+ m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
			+ m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

			- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
			- m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
			- m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

			- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
			- m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
			- m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

			+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
			+ m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
			+ m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

			+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
			+ m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
			+ m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

			- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
			- m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
			- m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

			- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
			- m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
			- m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

			+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
			+ m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
			+ m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

		Matrix4x4 result = {};

		float recpDeterminant = 1.0f / determinant;

		result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
			m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
			m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
		result.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
			m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
			m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
		result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
			m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
			m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) * recpDeterminant;
		result.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
			m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
			m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) * recpDeterminant;

		result.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
			m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
			m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
		result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
			m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
			m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
		result.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
			m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
			m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) * recpDeterminant;
		result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
			m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
			m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) * recpDeterminant;

		result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
			m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
			m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) * recpDeterminant;
		result.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
			m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] +
			m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) * recpDeterminant;
		result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
			m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
			m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) * recpDeterminant;
		result.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
			m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
			m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) * recpDeterminant;

		result.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
			m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
			m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) * recpDeterminant;
		result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
			m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
			m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) * recpDeterminant;
		result.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
			m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
			m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) * recpDeterminant;
		result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
			m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
			m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) * recpDeterminant;

		return result;
	}

	// MakeOrthographicMatrix:
	// - 直行投影行列を作成する（UI / スプライト用途で使用）。
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
	{
		Matrix4x4 result{ 0 };
		result.m[0][0] = 2.0f / (right - left);
		result.m[1][1] = 2.0f / (top - bottom);
		result.m[2][2] = 1.0f / (farClip - nearClip);
		result.m[3][0] = (left + right) / (left - right);
		result.m[3][1] = (top + bottom) / (bottom - top);
		result.m[3][2] = nearClip / (nearClip - farClip);
		result.m[3][3] = 1.0f;
		return result;
	}

	// TransformNormal:
	// - 法線ベクトルを行列で変換するユーティリティ。
	Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix)
	{
		Vector3 result;
		result.x = matrix.m[0][0] * vector.x + matrix.m[1][0] * vector.y + matrix.m[2][0] * vector.z;
		result.y = matrix.m[0][1] * vector.x + matrix.m[1][1] * vector.y + matrix.m[2][1] * vector.z;
		result.z = matrix.m[0][2] * vector.x + matrix.m[1][2] * vector.y + matrix.m[2][2] * vector.z;

		return result;
	}

	// Length / Normalize:
	// - ベクトル長や正規化を行う。Normalize は length が 0 の場合に分母ゼロになるため、
	//   呼び出し側で零ベクトルを渡さないこと、またはここでのガードを追加することを推奨する。
	float Length(const Vector3& v) {
		float result;
		result = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		return result;
	}

	Vector3 Normalize(const Vector3& v) {
		float length = Length(v);
		Vector3 result;
		result.x = v.x / length;
		result.y = v.y / length;
		result.z = v.z / length;
		return result;
	}
}