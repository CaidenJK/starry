#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec4 fragNorm;
layout(location = 2) out vec2 fragTexCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

vec4 localToWorldNorm(vec3 localNormal, mat4 modelMatrix)
{
    mat3 normalMatrix = mat3(transpose(inverse(ubo.model)));
    vec3 worldNormal = normalize(normalMatrix * localNormal);
    return vec4(worldNormal, 0.0);
}

void main() 
{
    mat4 mvpMatrix = ubo.proj * ubo.view * ubo.model;
    gl_Position = mvpMatrix * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragNorm = localToWorldNorm(inNorm, ubo.model);
    fragTexCoord = inTexCoord;
}