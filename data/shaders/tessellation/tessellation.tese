#version 450

layout (std140, set = 0, binding = 0) uniform UBO_viewProjection 
{
	mat4 projection;
	mat4 view;
} ubo;

layout (push_constant) uniform PushConstants {
	 mat4 world;
	 mat4 worldInv;
} pushConstants;

layout(quads, equal_spacing, cw) in;

layout (location = 0) in vec3 InNormalL[];
layout (location = 1) in vec2 InTex[];
 
layout (location = 0) out vec3 OutNormalL;
layout (location = 1) out vec2 OutTex;

void main()
{
	// Interpolate UV coordinates
	vec2 uv1 = mix(InTex[0], InTex[1], gl_TessCoord.x);
	vec2 uv2 = mix(InTex[3], InTex[2], gl_TessCoord.x);
	OutTex = mix(uv1, uv2, gl_TessCoord.y);

	// Interpolate normals
	vec3 n1 = mix(InNormalL[0], InNormalL[1], gl_TessCoord.x);
	vec3 n2 = mix(InNormalL[3], InNormalL[2], gl_TessCoord.x);
	OutNormalL = mix(n1, n2, gl_TessCoord.y);

	// Interpolate positions
	vec4 pos1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 pos2 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);
	vec4 pos = mix(pos1, pos2, gl_TessCoord.y);

	// Displace
	//pos.y -= textureLod(displacementMap, outUV, 0.0).r * ubo.displacementFactor;

	// Perspective projection
	gl_Position = ubo.projection * ubo.view * pushConstants.world * pos;
}