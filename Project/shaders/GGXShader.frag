#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 150
#define PI 3.14159265358979323846f

layout(binding = 1) uniform GlobalUniformBufferObject {
    vec4[MAX_LIGHTS] lightPos;
    vec4 lightColor;
    vec3 eyePos;//cam position
    int numberOfLights;
} gubo;

layout(binding = 2) uniform sampler2D tex;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;
layout(location = 3) in float roughness;
layout(location = 4) in float ao;
layout(location = 5) in float metallic;//parameter that mixes the diffuse with the specular term

layout(location = 0) out vec4 outColor;

// Point light (multiple lights)
// Cook-Torrance GGX

void main() {
        
	vec3 norm = normalize(fragNorm);
	vec3 eyeDir = normalize(gubo.eyePos - fragPos);
    vec3 lightDirs[MAX_LIGHTS];
    vec3 decayFactors[MAX_LIGHTS];
    vec3 lightColorsDecayed[MAX_LIGHTS];

    //parameters for the point light:
    const float beta = 4.0f;//decay factor (exponent) for
    const float g = 0.5f;//distance at which the light reduction is exactly 1

    //The BRDF is expressed in terms of diffuse + specular
    vec3 sumDiffuse = vec3(0.0, 0.0, 0.0);//light diffuses
    vec3 sumSpecular = vec3(0.0, 0.0, 0.0);//light due to reflections

    vec3 sumLightColorsRgb = vec3(0.0, 0.0, 0.0);//to graduate the light based on light position
    vec3 Ms = vec3(1.0f);//color of the reflected light (with)
    float F0 = 0.3f;//The fresnel term depends to this parameter F0
    vec3 albedo = texture(tex, fragUV).rgb;//main color of the surface
    for(int i = 0; i < gubo.numberOfLights; i++){
        vec3 lightVert = vec3(gubo.lightPos[i]) - fragPos;//a vector witch represents the distance between the point you are analyzing and the light
        vec3 lightDir = lightVert / length(lightVert);//normalize lightVert
        vec3 decayFactor = gubo.lightColor.rgb * pow((g / length(lightVert)), beta);//decay factor for the point light model

        vec3 lightColorsDecayed = decayFactor * gubo.lightColor.rgb;
        vec3 diffuse = albedo * 0.99f * clamp(dot(norm, lightDir), 0.0, 1.0);//we use Lambert to calculate the diffuse term

        float K = 1.0f - metallic;
        vec3 halfVec = normalize(eyeDir + lightDir);//a vector that is in the middle between the viewer direction and the light
        float D = pow(roughness, 2) / (PI * pow((clamp((pow(dot(norm, halfVec), 2)), 0, 1) * (pow(roughness, 2) - 1) ) + 1, 2));//the Distribution term (it accounts for the roughness of the surface)
        float F = F0 + ((1 - F0) * (pow((1 - clamp((dot(eyeDir, halfVec)), 0, 1)), 5)));//Fresnel term (It defines how the light response changes with the angle of incidence with respect to the viewer)
        float G = dot((2 / (1 + (sqrt(1 + (pow(roughness, 2) * ((1 - (pow((dot(norm, eyeDir)), 2))) / (pow((dot(norm, eyeDir)), 2)))))))), (2 / (1 + (sqrt(1 + (pow(roughness, 2) * ((1 - (pow((dot(norm, lightDir)), 2))) / (pow((dot(norm, lightDir)), 2)))))))));//geometric term for the Cook-Torrance GGX 
        vec3 specular = Ms * ((D * F * G) / (4 * clamp((dot(eyeDir, norm)), 0, 1)));//calculate specular term

        sumDiffuse += K * diffuse;
        sumSpecular += (1-K) * specular;
        sumLightColorsRgb += lightColorsDecayed.rgb;//this term depoends by the dacay
    }
    vec3 ambient = albedo * 0.05f * ao;

    //assign to each pixel a color
    outColor = vec4(clamp((sumDiffuse + sumSpecular) * sumLightColorsRgb + ambient, 0.0, 1.0), 1.0f);
}
