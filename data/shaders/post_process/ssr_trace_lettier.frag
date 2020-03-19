#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 0) uniform sampler2D lightSampler;
layout (set = 0, binding = 1) uniform sampler2D specularSampler;
layout (set = 0, binding = 2) uniform sampler2D normalViewSampler;
layout (set = 0, binding = 3) uniform sampler2D normalWorldSampler;
layout (set = 0, binding = 4) uniform sampler2D positionSampler;

layout (location = 0) in vec2 InTex;

layout (location = 0) out vec4 OutFragColor;

layout (std140, set = 1, binding = 0) uniform UBO 
{
	mat4 view;
	mat4 projection;
} ubo;

void main() 
{
  // HACK:
  vec2 dummy = InTex;
  vec4 dummy2 = texture(lightSampler, InTex);

  float maxDistance = 1500;
  float resolution  = 0.2;
  int   steps       = 5;
  float thickness   = 0.5;

  vec2 texSize  = textureSize(positionSampler, 0).xy;
  vec2 texCoord = InTex;//gl_FragCoord.xy / texSize; // InTex

  vec4 uv = vec4(0);

  vec4 positionFrom = texture(positionSampler, texCoord);
  positionFrom = ubo.view * positionFrom;

  vec4 mask         = texture(specularSampler,     texCoord);

  // Temp
  vec3 normalWorld           = normalize(texture(normalWorldSampler, texCoord).xyz);

  if ( positionFrom.w                  <= 0
     //|| enabled.x                      != 1
     || dot(mask.rgb, vec3(1.0 / 3.0)) <= 0.0
     || normalWorld.y < 0.7f // To get ground only
     ) { OutFragColor = uv; return; }

  vec3 unitPositionFrom = normalize(positionFrom.xyz);
  vec3 normal           = normalize(texture(normalViewSampler, InTex).xyz * 2.0f - 1.0f);
  vec3 pivot            = normalize(reflect(unitPositionFrom, normal));

  vec4 positionTo = positionFrom;

  vec4 startView = vec4(positionFrom.xyz + (pivot *           0), 1);
  vec4 endView   = vec4(positionFrom.xyz + (pivot * maxDistance), 1);

  vec4 startFrag      = startView;
       startFrag      = ubo.projection * startFrag;
       startFrag.xyz /= startFrag.w;
       startFrag.xy   = startFrag.xy * 0.5 + 0.5;
       startFrag.xy  *= texSize;

  vec4 endFrag      = endView;
       endFrag      = ubo.projection * endFrag;
       endFrag.xyz /= endFrag.w;
       endFrag.xy   = endFrag.xy * 0.5 + 0.5;
       endFrag.xy  *= texSize;

  vec2 frag  = startFrag.xy;
       uv.xy = frag / texSize;

  float deltaX    = endFrag.x - startFrag.x;
  float deltaY    = endFrag.y - startFrag.y;
  float useX      = abs(deltaX) >= abs(deltaY) ? 1 : 0;
  float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0, 1);
  vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);

  float search0 = 0;
  float search1 = 0;

  int hit0 = 0;
  int hit1 = 0;

  float viewDistance = startView.y;
  float depth        = thickness;

  float i = 0;

  for (i = 0; i < int(delta); ++i) {
    frag      += increment;
    uv.xy      = frag / texSize;
    positionTo = texture(positionSampler, uv.xy);
    positionTo = ubo.view * positionTo;

    search1 =
      mix
        ( (frag.y - startFrag.y) / deltaY
        , (frag.x - startFrag.x) / deltaX
        , useX
        );

    search1 = clamp(search1, 0, 1);

    viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
    depth        = viewDistance - positionTo.y;

    if (depth > 0 && depth < thickness) {
      hit0 = 1;
      break;
    } else {
      search0 = search1;
    }
  }

  search1 = search0 + ((search1 - search0) / 2);

  steps *= hit0;

  for (i = 0; i < steps; ++i) {
    frag       = mix(startFrag.xy, endFrag.xy, search1);
    uv.xy      = frag / texSize;
    positionTo = texture(positionSampler, uv.xy);
    positionTo = ubo.view * positionTo;

    viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
    depth        = viewDistance - positionTo.y;

    if (depth > 0 && depth < thickness) {
      hit1 = 1;
      search1 = search0 + ((search1 - search0) / 2);
    } else {
      float temp = search1;
      search1 = search1 + ((search1 - search0) / 2);
      search0 = temp;
    }
  }

  float visibility =
      hit1
    * positionTo.w
    * ( 1
      - max
         ( dot(-unitPositionFrom, pivot)
         , 0
         )
      )
    * ( 1
      - clamp
          ( depth / thickness
          , 0
          , 1
          )
      )
    * ( 1
      - clamp
          (   length(positionTo - positionFrom)
            / maxDistance
          , 0
          , 1
          )
      )
    * (uv.x < 0 || uv.x > 1 ? 0 : 1)
    * (uv.y < 0 || uv.y > 1 ? 0 : 1);

  visibility = clamp(visibility, 0, 1);

  uv.ba = vec2(visibility);

  OutFragColor = uv;
  OutFragColor = positionFrom;
}