#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

layout (location = 0) in vec2 InTex;
layout (location = 0) out vec4 OutFragColor;

layout (std140, set = 0, binding = 0) uniform UBO_brush 
{
    vec2 pos;
    float radius;
    float strength;
    int mode; // 0 = height, 1 = blend
    int operation; // 0 = add, 1 = remove
} ubo_brush;

void main() 
{
    vec2 center = ubo_brush.pos;
    //center = vec2(0.5, 0.5);
    float height = smoothstep(ubo_brush.radius, 0.0, distance(center, InTex));

    if (ubo_brush.operation == 0) // Increase amplitude
        height *= -1;

    OutFragColor = vec4(height / (300 - ubo_brush.strength));
}