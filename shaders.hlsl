//=========================================================
// @file shaders.hlsl
// @auth Kota Tamaoki
// @date 2022/12/03
//=========================================================

cbuffer cbTansMatrix : register(b0)
{
    float4x4 g_worldViewProj; // ワールドビュー射影行列
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 pos = float4(input.position, 1.0f);
    float4 norm = float4(input.normal, 0.0f);

    output.position = mul(pos, g_worldViewProj);
    output.normal = mul(norm, g_worldViewProj);
    output.color = input.color;
    output.uv = input.uv;

    return output;
}

// テクスチャ情報
Texture2D g_texture : register(t0); // 基本色
SamplerState g_sampler : register(s0); // サンプラ

// ピクセルシェーダ入力パラメータ
typedef VS_OUTPUT PS_INPUT; // 置き換え
// 頂点シェーダのアウトプットがそのままピクセルシェーダのインプットになるので、
// 分かりやすいように名前の置き換えを行う。

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	// そのまま出力
    return g_texture.Sample(g_sampler, input.uv) * input.color;
}