#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 150

layout(binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;//model view projection matrix (3 transforms)
	mat4 mMat;//model matrix
	mat4 nMat;//normal matrix
	float roughness;
	float ao;
	float metallic;
} ubo;

layout(binding = 1) uniform GlobalUniformBufferObject {
	vec4 lightColor;
	vec3 eyePos;
} gubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragUV;
layout(location = 3) out float roughness;
layout(location = 4) out float ao;
layout(location = 5) out float metallic;

void main() {

	fragPos = (ubo.mMat * vec4(inPosition, 1.0)).xyz;
	fragNorm = (ubo.nMat * vec4(inNorm, 0.0)).xyz;
	fragUV = inUV;

	//pass to fragment shader
	roughness = ubo.roughness;
	ao = ubo.ao;
	metallic = ubo.metallic;

	gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);
}
