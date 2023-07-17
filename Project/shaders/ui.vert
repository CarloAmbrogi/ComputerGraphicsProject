#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} uboUI;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragPosUI;
layout(location = 1) out vec3 fragNormUI;
layout(location = 2) out vec2 fragUVUI;

void main() {
	gl_Position = uboUI.mvpMat * vec4(inPosition, 1.0);
	fragPosUI = (uboUI.mMat * vec4(inPosition, 1.0)).xyz;
	fragNormUI = inNorm;
	fragUVUI = inUV;
}
