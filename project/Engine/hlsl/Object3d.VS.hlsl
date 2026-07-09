#include "object3d.hlsli"
struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};
ConstantBuffer<TransformationMatrix> gTransformationmatrix : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.texcoord = input.texcoord;
    // 位置をWVPで変換
    output.position = mul(input.position, gTransformationmatrix.WVP);
    // 法線をワールド行列の上位3x3で変換して正規化して出力
    float3 worldNormal = normalize(mul(input.normal, (float3x3) gTransformationmatrix.World));
    output.normal = worldNormal;
    return output;
}