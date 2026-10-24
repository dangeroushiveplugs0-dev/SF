struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float2 texCoord : TEXCOORD0;
};

struct VertexOutput
{
    [[vk::location(0)]] float3 worldPosition : TEXCOORD0;
    [[vk::location(1)]] float3 worldNormal : TEXCOORD1;
    [[vk::location(2)]] float2 texCoord : TEXCOORD2;
    float4 position : SV_Position;
};

[[vk::binding(0, 0)]]
cbuffer CameraBlock
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    float4 world = mul(model, float4(input.position, 1.0));
    output.worldPosition = world.xyz;
    output.worldNormal = normalize(mul(transpose(inverse((float3x3)model)), input.normal));
    output.texCoord = input.texCoord;
    output.position = mul(projection, mul(view, world));
    return output;
}
