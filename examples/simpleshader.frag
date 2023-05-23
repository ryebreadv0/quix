#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 texCoord;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor*texture(texSampler, texCoord).rgb,1.0);
}