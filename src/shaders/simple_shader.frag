#version 450

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec4 inNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

float lightIntesity(vec3 sunDirection, vec3 surfaceNormal)
{
    vec3 sun = normalize(sunDirection);
    vec3 surface = normalize(surfaceNormal);

    float dotProduct = dot(sun, surface);
    if (dotProduct > 0.0) {
        dotProduct = 0.0;
    } 
    return (-dotProduct);
}

void main() {
    vec3 sunDirection = vec3(0.1, -1.0, 0.1);
    vec3 ambientLight = vec3(0.013, 0.015, 0.03);
    // texture(texSampler, fragTexCoord)
    outColor = vec4(0.9, 0.9, 0.9, 1.0) * lightIntesity(sunDirection, inNorm.xyz) + vec4(ambientLight, 1.0);
}