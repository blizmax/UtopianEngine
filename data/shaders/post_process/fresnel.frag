#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 0) uniform sampler2D reflectionSampler;
layout (set = 0, binding = 1) uniform sampler2D refractionSampler;

layout (location = 0) in vec2 InTex;

layout (location = 0) out vec4 OutFragColor;

void main() 
{
    vec3 reflectionColor = texture(reflectionSampler, InTex).rgb;
    vec3 refractionColor = texture(refractionSampler, InTex).rgb;

    vec3 finalColor = mix(reflectionColor, refractionColor, 0.5f);

    //OutFragColor = vec4(reflectionColor / 2.0f, 1.0f);
    OutFragColor = vec4(finalColor / 2.0f, 1.0f);
}