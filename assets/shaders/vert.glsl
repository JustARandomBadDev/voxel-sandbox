#version 450

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(binding = 0) uniform UniformBufferObject {
    vec3 cameraPos;
    mat4 matrix;
    Light sunLight;
    Light moonLight;
 } ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in float inShininess;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out float fragShininess;

void main() {
    fragPos = inPosition;
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;
    fragShininess = inShininess;

    gl_Position = ubo.matrix * vec4(inPosition, 1.0);
}