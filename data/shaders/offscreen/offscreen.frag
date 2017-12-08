#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPosW;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec4 outFragColor;

void main(void)
{
	outFragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);	
}