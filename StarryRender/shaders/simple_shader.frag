#version 450

layout(location = 0) in dvec3 inPosition;
layout(location = 2) in vec3 inColor;

void main() {
    outColor = vec4(inColor, 1.0);
}