#version 450

layout (location = 0) in vec3 InPosL;
layout (location = 1) in vec3 InColor;
layout (location = 2) in vec3 InNormalL;
layout (location = 3) in vec2 InTex;
layout (location = 4) in vec3 InTangentL;
layout (location = 5) in vec3 InBitangentL;

layout (std140, set = 0, binding = 0) uniform UBO_viewProjection 
{
	// Camera 
	mat4 projection;
	mat4 view;
} per_frame_vs;

layout (push_constant) uniform PushConstants {
	 mat4 world;
	 vec4 color;
	 
	 // These exceeds the 128 byte limit
	 // vec2 textureTiling;
	 // vec2 pad;
} pushConstants;

layout (location = 0) out vec4 OutColor;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main() 
{
	OutColor = pushConstants.color;

	// Note: workaround to avoid glslang to optimize unused inputs
	vec3 temp = InColor;
	temp = InNormalL;
	vec2 temp1 = InTex;
	vec3 temp2 = InTangentL;
	temp2 = InBitangentL;

	gl_Position = per_frame_vs.projection * per_frame_vs.view * pushConstants.world * vec4(InPosL.xyz, 1.0);
}