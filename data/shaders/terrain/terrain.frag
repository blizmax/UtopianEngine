#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPosW;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outFragColor;

layout (set = 0, binding = 2) uniform sampler3D texture3d;

layout (std140, set = 0, binding = 1) uniform UBO 
{
	vec3 eyePos;
	float padding;
	float fogStart;
	float fogDistance;
} per_frame_ps;

void main(void)
{
	vec3 lightVec = vec3(1, 1, 1);
	lightVec = normalize(lightVec);

	float diffuseFactor = max(0, dot(lightVec, inNormal));

	float ambientFactor = 0.1;
	vec3 color = inColor * ambientFactor + diffuseFactor * inColor;

	// Apply fogging.
	float distToEye = length(per_frame_ps.eyePos + inPosW); // TODO: NOTE: This should be "-". Related to the negation of the world matrix push constant.
	float fogLerp = clamp((distToEye - per_frame_ps.fogStart) / per_frame_ps.fogDistance, 0.0, 1.0); 

	// Blend the fog color and the lit color.
	color = mix(color, vec3(0.2), fogLerp);
	outFragColor = vec4(color,  1.0);
	//outFragColor = vec4(inNormal, 1.0);

	//outFragColor = texture(texture3d, vec3(0, 0, 0)); 
	//outFragColor = vec4(texelFetch(texture3d, ivec3(4, 15, 5),  0).rgb, 1.0f);
	//outFragColor = vec4(texture(texture3d, inPosW,  0).rgb, 1.0f);
}