#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform GlobalUniformBufferObject {
	vec3[2] lightPos;
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
    
    vec3[2] pointPos;
    for(int i = 0; i < 2; i++){
        pointPos[i] = gubo.lightPos[i];
    }
    
    vec3[2] decayFactor;
    for(int i = 0; i < 2; i++){
        decayFactor[i] = gubo.lightColor.rgb * pow((g / length(pointPos[i] - fragPos)), beta);
    }
    
    vec3[2] lightDir;
    vec3[2] lightColor;
    for(int i = 0; i < 2; i++){
        lightDir[i] = (pointPos[i] - fragPos) / length(pointPos[i] - fragPos);
    }
    for(int i = 0; i < 2; i++){
        lightColor[i] = decayFactor[i] * gubo.lightColor.rgb;
    }

    vec3[2] Diffuse;
    vec3[2] Specular;
    for(int i = 0; i < 2; i++){
        Diffuse[i] = texture(tex, fragUV).rgb * 0.99f * clamp(dot(Norm, lightDir[i]),0.0,1.0);
    }
    for(int i = 0; i < 2; i++){
        Specular[i] = vec3(pow(clamp(dot(Norm, normalize(lightDir[i] + EyeDir)),0.0,1.0), 160.0f));
    }
    
    vec3 sumDiffuse = Diffuse[0];
    vec3 sumSpecular = Specular[0];
    vec3 sumLightColorRgb = lightColor[0].rgb;
    for(int i = 1; i < 2; i++){
        sumDiffuse += Diffuse[i];
        sumSpecular += Specular[i];
        sumLightColorRgb += lightColor[i].rgb;
    }
    
    outColor = vec4(clamp((sumDiffuse + sumSpecular) * sumLightColorRgb,0.0,1.0), 1.0f);
    
}
