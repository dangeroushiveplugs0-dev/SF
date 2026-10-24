#version 450

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 worldNormal;
layout(location = 2) in vec2 texCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1, std140) uniform LightingBlock {
    vec4 lightPosition;
    vec4 lightColor;
    vec4 viewPosition;
    vec4 objectColor;
    float ambientStrength;
    float specularStrength;
} lighting;

void main()
{
    vec3 normal = normalize(worldNormal);
    vec3 lightDirection = normalize(lighting.lightPosition.xyz - worldPosition);
    vec3 viewDirection = normalize(lighting.viewPosition.xyz - worldPosition);
    vec3 reflectedDirection = reflect(-lightDirection, normal);

    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    float specularFactor = pow(max(dot(viewDirection, reflectedDirection), 0.0), 32.0);

    vec3 ambient = lighting.ambientStrength * lighting.lightColor.rgb;
    vec3 diffuse = diffuseFactor * lighting.lightColor.rgb;
    vec3 specular = lighting.specularStrength * specularFactor * lighting.lightColor.rgb;
    vec3 result = (ambient + diffuse + specular) * lighting.objectColor.rgb;

    outColor = vec4(result, lighting.objectColor.a);
}
