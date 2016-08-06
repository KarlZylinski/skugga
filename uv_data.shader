cbuffer ConstantBuffer : register(b0)
{
    float4x4 model_view_projection;
    float4x4 model;
    float4x4 projection;
    float4 sun_position;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 vertex_pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct POut
{
    float4 position : SV_TARGET0;
    float4 normal : SV_TARGET1;
};

VOut VShader(float4 position : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, float4 color : COLOR)
{
    VOut output;

    output.position = mul(projection, float4(uv, 0, 1));
    output.vertex_pos = position;
    output.normal = normal;
    output.uv = uv;
    output.color = color;

    return output;
}

POut PShader(float4 position : SV_POSITION, float4 vertex_pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, float4 color : COLOR)
{
    POut output;
    output.position = mul(model, vertex_pos);
    output.normal = mul(model, normal);
    return output;
}
