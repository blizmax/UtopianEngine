#pragma once
#include "core/Transform.h"
#include "LightData.h"
#include "imgui/imgui.h"
#include <string>
#include <vector>

namespace Utopian
{
	class CTransform;
	class CRenderable;
	class CLight;
	class CRigidBody;
	class CCatmullSpline;

	class ComponentInspector
	{
	public:
		ComponentInspector();
		virtual ~ComponentInspector();

		virtual void UpdateUi() = 0;
	private:
	};

	class TransformInspector : public ComponentInspector
	{
	public:
		TransformInspector(CTransform* transform);

		virtual void UpdateUi() override;
	private:
		CTransform* mComponent;
		Transform mTransform;
	};

	class RenderableInspector : public ComponentInspector
	{
	public:
		struct TextureInfo
		{
			TextureInfo(ImTextureID id, std::string _path) {
				textureId = id;
				path = _path;
			}

			ImTextureID textureId;
			std::string path;
		};

		RenderableInspector(CRenderable* renderable);
		~RenderableInspector();

		virtual void UpdateUi() override;
	private:
		CRenderable* mRenderable;
		glm::ivec2 mTextureTiling;
		bool mDeferred;
		bool mColor;
		bool mBoundingBox;
		bool mDebugNormals;
		bool mWireframe;

		// For ImGui debug drawing
		std::vector<TextureInfo> textureInfos;
	};

	class LightInspector : public ComponentInspector
	{
	public:
		LightInspector(CLight* light);

		virtual void UpdateUi() override;
	private:
		CLight* mLight;
		Utopian::LightData mLightData;
		int mType;
	};

	class RigidBodyInspector : public ComponentInspector
	{
	public:
		RigidBodyInspector(CRigidBody* rigidBody);

		virtual void UpdateUi() override;
	private:
		CRigidBody* mRigidBody;
	};

	class CatmullSplineInspector : public ComponentInspector
	{
	public:
		CatmullSplineInspector(CCatmullSpline* catmullSpline);

		virtual void UpdateUi() override;
	private:
		CCatmullSpline* mCatmullSpline;
	};
}
