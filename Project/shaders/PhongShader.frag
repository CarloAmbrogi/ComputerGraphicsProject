#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform GlobalUniformBufferObject {
	vec3 lightPos;
	vec4 lightColor;
	vec3 eyePos;
} gubo;

layout(binding = 2) uniform sampler2D tex;

void main() {
        
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    
    //point light
    const float beta = 2.0f;
    const float g = 1.5;
    
    vec3 pointPos = gubo.lightPos;
    
    vec3 decayFactor = gubo.lightColor.rgb * pow((g / length(pointPos - fragPos)), beta);
    
    vec3 lightDir = (pointPos - fragPos) / length(pointPos - fragPos);
    vec3 lightColor = decayFactor * gubo.lightColor.rgb;

    vec3 Diffuse = texture(tex, fragUV).rgb * 0.99f * clamp(dot(Norm, lightDir),0.0,1.0);
    vec3 Specular = vec3(pow(clamp(dot(Norm, normalize(lightDir + EyeDir)),0.0,1.0), 160.0f));
    
    outColor = vec4(clamp((Diffuse + Specular) * lightColor.rgb,0.0,1.0), 1.0f);
    
}
