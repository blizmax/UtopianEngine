#version 450

layout (location = 0) in vec3 InColor;
layout (location = 1) in vec3 InPosW;
layout (location = 2) in vec3 InNormalW;
layout (location = 3) in vec2 InTex;
layout (location = 4) in vec3 InNormalV;
layout (location = 5) in vec2 InTextureTiling;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outAlbedo;

// Output normals in view space so that the SSAO pass can use them.
// Should be reworked so that you don't have to use two separate textures
// for normals in world space vs view space.
layout (location = 3) out vec4 outNormalV;

layout (set = 1, binding = 0) uniform sampler2D textureSampler[3];
layout (set = 1, binding = 1) uniform sampler2D normalSampler;

const float NEAR_PLANE = 10.0f; //todo: specialization const
const float FAR_PLANE = 256000.0f; //todo: specialization const 

float linearDepth(float depth)
{
	float z = depth * 2.0f - 1.0f; 
	return (2.0f * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE));	
}

void main() 
{
	vec4 normal = vec4(normalize(InNormalW), 1.0f);
	normal.y *= -1;
	outPosition = vec4(InPosW, linearDepth(gl_FragCoord.z));
	outNormal = normal;
	outNormalV = vec4(normalize(InNormalV) * 0.5 + 0.5, 1.0f);

	vec4 color = vec4(1.0f);
	vec4 flatColor = vec4(1.0f);
	vec4 grassColor = texture(textureSampler[0], InTex * InTextureTiling);
	vec4 rockColor = texture(textureSampler[1], InTex * InTextureTiling);
	vec4 snowColor = texture(textureSampler[2], InTex * InTextureTiling);

	// Use snow at high altitudes
	float y = -InPosW.y; // Note: negated
	const float fadeStart = 800.0f;
	const float fadeEnd = 900.0f;
	if (y > fadeEnd)
		flatColor = snowColor;
	else if (y > fadeStart && y <= fadeEnd)
		flatColor = mix(grassColor, snowColor, (y - fadeStart) / (fadeEnd - fadeStart));
	else
		flatColor = grassColor;

	// Slope texture calculation
	float slope = 1 - normal.y;
	if (slope < 0.3f)
		color = flatColor;
	else if (slope >= 0.3f && slope < 0.45f)
		color = mix(flatColor, rockColor, (slope - 0.3f) / (0.45f - 0.3f));
	else
		color = rockColor;
	

	outAlbedo = color;

	if (InColor != vec3(1.0f))
		outAlbedo = vec4(InColor, 1.0f);
}