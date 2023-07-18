#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 150

layout(binding = 1) uniform GlobalUniformBufferObject {
    vec4[MAX_LIGHTS] lightPos;
    vec4 lightColor;
    vec3 eyePos;
    int numberOfLights;
} gubo;

layout(binding = 2) uniform sampler2D tex;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

void main() {
        
	vec3 norm = normalize(fragNorm);
	vec3 eyeDir = normalize(gubo.eyePos - fragPos);
    vec3 lightDirs[MAX_LIGHTS];
    vec3 decayFactors[MAX_LIGHTS];
    vec3 lightColorsDecayed[MAX_LIGHTS];

    //point light
    const float beta = 4.0f;
    const float g = 0.5f;
    for(int i = 0; i < gubo.numberOfLights; i++){
        vec3 lightVert = vec3(gubo.lightPos[i]) - fragPos;
        vec3 decayFactor = gubo.lightColor.rgb * pow((g / length(lightVert)), beta);
        lightColorsDecayed[i] = decayFactor * gubo.lightColor.rgb;
    }

    vec3 sumDiffuse = vec3(0.0, 0.0, 0.0);
    vec3 sumSpecular = vec3(0.0, 0.0, 0.0);
    vec3 sumLightColorsRgb = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < gubo.numberOfLights; i++){
        vec3 lightVert = vec3(gubo.lightPos[i]) - fragPos;
        vec3 lightDirs = lightVert / length(lightVert);
        vec3 diffuse = texture(tex, fragUV).rgb * 0.99f * clamp(dot(norm, lightDirs), 0.0, 1.0);
        vec3 specular = vec3(pow(clamp(dot(norm, normalize(lightDirs + eyeDir)),0.0 ,1.0), 60.0f));
        sumDiffuse += diffuse;
        sumSpecular += specular;
        sumLightColorsRgb += lightColorsDecayed[i].rgb;
    }

    outColor = vec4(clamp((sumDiffuse + sumSpecular) * sumLightColorsRgb, 0.0, 1.0), 1.0f);
}
