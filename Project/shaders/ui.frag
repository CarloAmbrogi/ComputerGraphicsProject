#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPosUI;
layout(location = 1) in vec3 fragNormUI;
layout(location = 2) in vec2 fragUVUI;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;

void main() {
    
    //show the texture color for the UI
    outColor = vec4((texture(tex, fragUVUI).rgb),1);
    
}
