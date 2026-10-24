struct FragmentInput
{
    [[vk::location(0)]] float3 worldPosition : TEXCOORD0;
    [[vk::location(1)]] float3 worldNormal : TEXCOORD1;
    [[vk::location(2)]] float2 texCoord : TEXCOORD2;
};

[[vk::location(0)]] float4 main(FragmentInput input) : SV_Target0
{
    float4 lightPosition;
    float4 lightColor;
    float4 viewPosition;
    float4 objectColor;
    float ambientStrength;
    float specularStrength;
    return objectColor;
}

[[vk::binding(1, 0)]]
cbuffer LightingBlock
{
    float4 lightPosition;
    float4 lightColor;
    float4 viewPosition;
    float4 objectColor;
    float ambientStrength;
    float specularStrength;
};
