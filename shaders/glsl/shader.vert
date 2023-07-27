#version 450 core
//#extension GL_KHR_vulkan_glsl: enable

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 fragColor;

layout( push_constant ) uniform constants
{
    vec2 offset;
    vec2 scale;
    // mat4 projViewModel;
} pc;

void main() {
    // gl_Position = pc.projViewModel * vec4(inPosition, 0.0, 1.0);
    vec2 position = inPosition * pc.scale + pc.offset;
	gl_Position = vec4(position, 0.0, 1.0);
    fragColor = inColor;
}