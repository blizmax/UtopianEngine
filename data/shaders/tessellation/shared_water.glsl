/** Contains descriptors shared between multiple shader stages. */

layout (std140, set = 0, binding = 6) uniform UBO_viewProjection 
{
	mat4 projection;
	mat4 view;
    vec4 frustumPlanes[6];
    vec3 eyePos;
    float time;
} ubo_camera;

layout (std140, set = 0, binding = 7) uniform UBO_settings
{
    vec2 viewportSize;
    float edgeSize; // The size in pixels that all edges should have
	float tessellationFactor;
    float amplitude;
    float textureScaling;
    float bumpmapAmplitude;
    int wireframe;
} ubo_settings;

float getHeight(vec2 texCoord)
{
    float height = 0.0f;

    return height;
}

vec3 getNormal(vec2 texCoord)
{
    vec3 normal = vec3(0, 1, 0);

    return normal;
}