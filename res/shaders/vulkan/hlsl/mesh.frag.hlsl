struct FragmentInput
{
    [[vk::location(0)]] float3 worldPosition : TEXCOORD0;
    [[vk::location(1)]] float3 worldNormal : TEXCOORD1;
    [[vk::location(2)]] float2 texCoord : TEXCOORD2;
};

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

float4 main(FragmentInput input) : SV_Target0
{
    float3 normal = normalize(input.worldNormal);
    float3 lightDirection = normalize(lightPosition.xyz - input.worldPosition);
    float3 viewDirection = normalize(viewPosition.xyz - input.worldPosition);
    float3 reflectedDirection = reflect(-lightDirection, normal);

    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    float specularFactor = pow(max(dot(viewDirection, reflectedDirection), 0.0), 32.0);

    float3 ambient = ambientStrength * lightColor.rgb;
    float3 diffuse = diffuseFactor * lightColor.rgb;
    float3 specular = specularStrength * specularFactor * lightColor.rgb;
    float3 result = (ambient + diffuse + specular) * objectColor.rgb;

    return float4(result, objectColor.a);
}
