#pragma once

#include <glm/glm.hpp>
#include "vulkan/Effect.h"
#include "vulkan/ShaderBuffer.h"
#include "vulkan/handles/Buffer.h"
#include "vulkan/PipelineInterface.h"

namespace Vulkan
{
	class Renderer;
	class DescriptorSetLayout;
	class DescriptorPool;
	class DescriptorSet;
	class Pipeline2;
	class ComputePipeline;
	class PipelineLayout;
	class VertexDescription;
	class Shader;
	class Texture;

	/** \brief 
	*
	* This effect expects the vertices to be in NDC space and simply applies a texture to them.
	* Should be used for rendering texture quads to the screen.
	**/
	class ScreenQuadEffect : public Effect
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec2 uv;
		};

		struct PushConstantBlock
		{
			glm::mat4 world;
		};

		ScreenQuadEffect();

		// Override the base class interfaces
		virtual void CreateDescriptorPool(Device* device);
		virtual void CreateVertexDescription(Device* device);
		virtual void CreatePipelineInterface(Device* device);
		virtual void CreateDescriptorSets(Device* device);
		virtual void CreatePipeline(Renderer* renderer);
		virtual void UpdateMemory(Device* device);

		DescriptorSet* mDescriptorSet0; // set = 0 in GLSL
	private:
	};
}