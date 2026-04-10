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

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in float fragShininess;

layout(location = 0) out vec4 outColor;

vec3 texColor = texture(texSampler, fragTexCoord).rgb;

void main() {
    // ambient
    vec3 ambient = ubo.sunLight.ambient * texColor;
  	
    // diffuse 
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(-ubo.sunLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = ubo.sunLight.diffuse * diff * texColor;
    
    // specular
    vec3 specular = vec3(0.0);
    if (fragShininess > 0) {
    vec3 viewDir = normalize(ubo.cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), fragShininess);
    specular = ubo.sunLight.specular * spec * texColor;
    }
        
    vec3 result = ambient + diffuse + specular;
    outColor = vec4(result, texture(texSampler, fragTexCoord).a);
}