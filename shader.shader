cbuffer ConstantBuffer : register(b0)
{
    float4x4 model_view_projection;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 vertex_pos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

VOut VShader(float4 position : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
    VOut output;

    output.position = mul(model_view_projection, position);
    output.vertex_pos = position;
    output.normal = normal;
    output.color = color;

    return output;
}


float4 PShader(float4 position : SV_POSITION, float4 vertex_pos : POSITION, float3 normal : NORMAL, float4 color : COLOR) : SV_TARGET
{
    float3 sun_pos = float3(290, 200, -300);
    float3 pos_to_sun = normalize(sun_pos - vertex_pos.xyz);
    float light = max(dot(pos_to_sun, normal) - 0.2, 0);
    return float4(color.xyz * light + color.xyz * 0.2, 1.0f);
}
