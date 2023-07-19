#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 150
#define PI 3.14159265358979323846f

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
layout(location = 3) in float roughness;
layout(location = 4) in float ao;
layout(location = 5) in float metallic;

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

    vec3 sumDiffuse = vec3(0.0, 0.0, 0.0);
    vec3 sumSpecular = vec3(0.0, 0.0, 0.0);
    vec3 sumLightColorsRgb = vec3(0.0, 0.0, 0.0);
    vec3 Ms = vec3(1.0f);
    float F0 = 0.3f;
    vec3 albedo = texture(tex, fragUV).rgb;
    for(int i = 0; i < gubo.numberOfLights; i++){
        vec3 lightVert = vec3(gubo.lightPos[i]) - fragPos;
        vec3 lightDir = lightVert / length(lightVert);
        vec3 decayFactor = gubo.lightColor.rgb * pow((g / length(lightVert)), beta);

        vec3 lightColorsDecayed = decayFactor * gubo.lightColor.rgb;
        vec3 diffuse = albedo * 0.99f * clamp(dot(norm, lightDir), 0.0, 1.0);

        float K = 1.0f - metallic;
        vec3 halfVec = normalize(eyeDir + lightDir);
        float D = pow(roughness, 2) / (PI * pow((clamp((pow(dot(norm, halfVec), 2)), 0, 1) * (pow(roughness, 2) - 1) ) + 1, 2));
        float F = F0 + ((1 - F0) * (pow((1 - clamp((dot(eyeDir, halfVec)), 0, 1)), 5)));
        float G = dot((2 / (1 + (sqrt(1 + (pow(roughness, 2) * ((1 - (pow((dot(norm, eyeDir)), 2))) / (pow((dot(norm, eyeDir)), 2)))))))), (2 / (1 + (sqrt(1 + (pow(roughness, 2) * ((1 - (pow((dot(norm, lightDir)), 2))) / (pow((dot(norm, lightDir)), 2)))))))));
        vec3 specular = Ms * ((D * F * G) / (4 * clamp((dot(eyeDir, norm)), 0, 1)));

        sumDiffuse += diffuse;
        sumSpecular += specular;
        sumLightColorsRgb += lightColorsDecayed.rgb;
    }
    vec3 ambient = albedo * 0.05f * ao;

    outColor = vec4(clamp((sumDiffuse + sumSpecular) * sumLightColorsRgb + ambient, 0.0, 1.0), 1.0f);
}
