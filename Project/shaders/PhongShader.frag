#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 100

layout(binding = 1) uniform GlobalUniformBufferObject {
    vec4[MAX_LIGHTS] lightPos;
    vec4 lightColor;
    vec3 eyePos;
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

    //point light
    const float beta = 4.0f;
    const float g = 0.5f;
    for(int i = 0; i < MAX_LIGHTS; i++) {
        vec3 lightVert = vec3(gubo.lightPos[i]) - fragPos;
        decayFactors[i] = gubo.lightColor.rgb * pow((g / length(lightVert)), beta);
        lightDirs[i] = lightVert / length(lightVert);
    }

    vec3[MAX_LIGHTS] lightColorsDecayed;
    for(int i = 0; i < MAX_LIGHTS; i++){
        lightColorsDecayed[i] = decayFactors[i] * gubo.lightColor.rgb;
    }

    vec3[MAX_LIGHTS] diffuse;
    vec3[MAX_LIGHTS] specular;
    for(int i = 0; i < MAX_LIGHTS; i++){
        diffuse[i] = texture(tex, fragUV).rgb * 0.99f * clamp(dot(norm, lightDirs[i]), 0.0, 1.0);
    }
    for(int i = 0; i < MAX_LIGHTS; i++){
        specular[i] = vec3(pow(clamp(dot(norm, normalize(lightDirs[i] + eyeDir)),0.0 ,1.0), 60.0f));
    }
    
    vec3 sumDiffuse = diffuse[0];
    vec3 sumSpecular = specular[0];
    vec3 sumLightColorsRgb = lightColorsDecayed[0].rgb;
    for(int i = 1; i < MAX_LIGHTS; i++){
        sumDiffuse += diffuse[i];
        sumSpecular += specular[i];
        sumLightColorsRgb += lightColorsDecayed[i].rgb;
    }
    
    outColor = vec4(clamp((sumDiffuse + sumSpecular) * sumLightColorsRgb, 0.0, 1.0), 1.0f);
    
}
