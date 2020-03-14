#include "ComponentInspector.h"
#include "ImGuiRenderer.h"
#include "vulkan/Debug.h"
#include "core/components/CTransform.h"
#include "core/components/CLight.h"
#include "core/components/CRenderable.h"
#include "core/components/CRigidBody.h"
#include "core/components/CCatmullSpline.h"
#include "core/components/Actor.h"
#include <glm/gtc/quaternion.hpp>
#include "vulkan/StaticModel.h"
#include "vulkan/handles/Texture.h"
#include "core/renderer/Renderer.h"

namespace Utopian
{
	ComponentInspector::ComponentInspector()
	{
	}

	ComponentInspector::~ComponentInspector()
	{

	}

	TransformInspector::TransformInspector(CTransform* transform)
	{
		mComponent = transform;
		mTransform = mComponent->GetTransform();
	}

	void TransformInspector::UpdateUi()
	{
		mTransform = mComponent->GetTransform();

		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Position", &mTransform.mPosition.x, 2);
			ImGui::SliderFloat3("Scale", &mTransform.mScale.x, 0.0f, 100.0f, "%.1f");

			static bool localRotate = true;
			ImGui::Checkbox("Local", &localRotate);

			glm::vec3 rotate = glm::vec3(0.0f);
			ImGui::Text("Rotation");
			ImGui::SameLine();
			ImGui::PushItemWidth(1.0f);
			ImGui::InputFloat("X", &rotate.x, 0.15);
			ImGui::SameLine();
			ImGui::InputFloat("Y", &rotate.y, 0.15);
			ImGui::SameLine();
			ImGui::InputFloat("Z", &rotate.z, 0.15);
			ImGui::PopItemWidth();

			mComponent->AddRotation(rotate, localRotate);
			mComponent->SetScale(mTransform.GetScale());
		}
	}

	RenderableInspector::RenderableInspector(CRenderable* renderable)
	{
		mRenderable = renderable;
		mTextureTiling = renderable->GetTextureTiling();
		mDeferred = renderable->HasRenderFlags(RenderFlags::RENDER_FLAG_DEFERRED);
		mColor = renderable->HasRenderFlags(RenderFlags::RENDER_FLAG_COLOR);
		mBoundingBox = renderable->HasRenderFlags(RenderFlags::RENDER_FLAG_BOUNDING_BOX);
		mDebugNormals = renderable->HasRenderFlags(RenderFlags::RENDER_FLAG_NORMAL_DEBUG);
		mWireframe = renderable->HasRenderFlags(RenderFlags::RENDER_FLAG_WIREFRAME);

		std::vector<Vk::Mesh*>& meshes = renderable->GetInternal()->GetModel()->mMeshes;

		for (auto& mesh : meshes)
		{
			std::vector<Vk::Texture*> textures = mesh->GetTextures();

			for (auto& texture : textures)
			{
				textureInfos.push_back(TextureInfo(gRenderer().GetUiOverlay()->AddTexture(texture->imageView), texture->GetPath()));
			}
		}
	}

	RenderableInspector::~RenderableInspector()
	{
		for (auto& textureInfo : textureInfos)
		{
			gRenderer().GetUiOverlay()->FreeTexture(textureInfo.textureId);
		}

		textureInfos.clear();
	}

	void RenderableInspector::UpdateUi()
	{
		if (ImGui::CollapsingHeader("Renderable", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Checkbox("Deferred", &mDeferred))
			{
				uint32_t flag = mRenderable->GetRenderFlags();

				if (mDeferred)
					flag |= RenderFlags::RENDER_FLAG_DEFERRED;
				else
					flag &= ~RenderFlags::RENDER_FLAG_DEFERRED;

				mRenderable->SetRenderFlags(flag);
			}

			if (ImGui::Checkbox("Color", &mColor))
			{
				uint32_t flag = mRenderable->GetRenderFlags();

				if (mColor)
					flag |= RenderFlags::RENDER_FLAG_COLOR;
				else
					flag &= ~RenderFlags::RENDER_FLAG_COLOR;

				mRenderable->SetRenderFlags(flag);
			}

			if (ImGui::Checkbox("Bounding box", &mBoundingBox))
			{
				uint32_t flag = mRenderable->GetRenderFlags();

				if (mBoundingBox)
					flag |= RenderFlags::RENDER_FLAG_BOUNDING_BOX;
				else
					flag &= ~RenderFlags::RENDER_FLAG_BOUNDING_BOX;

				mRenderable->SetRenderFlags(flag);
			}

			if (ImGui::Checkbox("Debug normals", &mDebugNormals))
			{
				uint32_t flag = mRenderable->GetRenderFlags();

				if (mDebugNormals)
					flag |= RenderFlags::RENDER_FLAG_NORMAL_DEBUG;
				else
					flag &= ~RenderFlags::RENDER_FLAG_NORMAL_DEBUG;

				mRenderable->SetRenderFlags(flag);
			}

			if (ImGui::Checkbox("Wireframe", &mWireframe))
			{
				uint32_t flag = mRenderable->GetRenderFlags();

				if (mWireframe)
					flag |= RenderFlags::RENDER_FLAG_WIREFRAME;
				else
					flag &= ~RenderFlags::RENDER_FLAG_WIREFRAME;

				mRenderable->SetRenderFlags(flag);
			}

			// If the renderable has a light then let the light inspector control the color
			glm::vec4 color = mRenderable->GetColor();
			if (!mRenderable->GetParent()->HasComponent<CLight>())
			{
				ImGui::ColorEdit3("Color", &color.x);
			}

			ImGui::SliderFloat("Brightness", &color.w, 0.0f, 100.0, "%.1f");
			mRenderable->SetColor(color);

			ImGui::SliderInt("Tiling", &mTextureTiling.x, 1, 200);

			mRenderable->SetTileFactor(glm::vec2(mTextureTiling.x, mTextureTiling.y));

			if (ImGui::CollapsingHeader("Meshes", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto& textureInfo : textureInfos)
				{
					ImGui::Text(textureInfo.path.c_str());
					if (ImGui::ImageButton(textureInfo.textureId, ImVec2(64, 64)))
					{
						// Nothing
					}
				}
			}
		}
	}

	LightInspector::LightInspector(CLight* light)
	{
		mLight = light;
		mLightData = mLight->GetLightData();
		mType = light->GetLightType();
	}

	void LightInspector::UpdateUi()
	{
		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Combo("Type", &mType, "Directional\0Point\0Spot\0");

			ImGui::SliderFloat3("Intensity", &mLightData.intensity.x, 0.0f, 1.0f);
			ImGui::SliderFloat3("Direction", &mLightData.direction.x, -1.0f, 1.0f);
			ImGui::SliderFloat3("Attenuation", &mLightData.att.x, 0.0f, 3.0f);
			ImGui::SliderFloat("Range", &mLightData.range, 0.0f, 10000.0f);
			ImGui::SliderFloat("Spot", &mLightData.spot, 0.0f, 30.0f);

			ImGui::ColorEdit3("Color", &mLightData.material.ambient.x);
			mLightData.material.diffuse = mLightData.material.ambient;
			mLightData.material.specular = mLightData.material.ambient;

			mLight->SetIntensity(mLightData.intensity);
			mLight->SetDirection(mLightData.direction);
			mLight->SetMaterial(mLightData.material);
			mLight->SetRange(mLightData.range);
			mLight->SetSpot(mLightData.spot);
			mLight->SetAttenuation(mLightData.att);
			mLight->SetType((Utopian::LightType)mType);
		}
	}

	RigidBodyInspector::RigidBodyInspector(CRigidBody* rigidBody)
	{
		mRigidBody = rigidBody;
	}

	void RigidBodyInspector::UpdateUi()
	{
		if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float mass = mRigidBody->GetMass();
			float friction = mRigidBody->GetFriction();
			float rollingFriction = mRigidBody->GetRollingFriction();
			float restitution = mRigidBody->GetRestitution();
			bool isKinematic = mRigidBody->IsKinematic();

			ImGui::Checkbox("Kinematic", &isKinematic);
			ImGui::SliderFloat("Mass", &mass, 0.0f, 100.0f);
			ImGui::SliderFloat("Friction", &friction, 0.01f, 10.0f);
			ImGui::SliderFloat("Rolling friction", &rollingFriction, 0.01f, 10.0f);
			ImGui::SliderFloat("Restitution", &restitution, 0.05f, 1.0f);

			mRigidBody->SetKinematic(isKinematic);
			mRigidBody->SetMass(mass);
			mRigidBody->SetFriction(friction);
			mRigidBody->SetRollingFriction(rollingFriction);
			mRigidBody->SetRestitution(restitution);
		}
	}

	CatmullSplineInspector::CatmullSplineInspector(CCatmullSpline* catmullSpline)
	{
		mCatmullSpline = catmullSpline;
	}

	void CatmullSplineInspector::UpdateUi()
	{
		if (ImGui::CollapsingHeader("Catmull spline", ImGuiTreeNodeFlags_DefaultOpen))
		{
			bool isActive = mCatmullSpline->IsActive();
			bool isDrawingDebug = mCatmullSpline->IsDrawingDebug();
			float timePerSegment = mCatmullSpline->GetTimePerSegment();

			ImGui::Text(mCatmullSpline->GetFilename().c_str());
			ImGui::SliderFloat("Time per segment", &timePerSegment, 200.0f, 10000.0f);
			ImGui::Checkbox("Active", &isActive);
			ImGui::Checkbox("Draw debug", &isDrawingDebug);

			if (ImGui::Button("Add control point"))
			{
				mCatmullSpline->AddControlPoint();
			}

			if (ImGui::Button("Remove control point"))
			{
				mCatmullSpline->RemoveLastControlPoint();
			}

			if (ImGui::Button("Save to file"))
			{
				mCatmullSpline->SaveControlPoints();
			}

			mCatmullSpline->SetActive(isActive);
			mCatmullSpline->SetDrawDebug(isDrawingDebug);
			mCatmullSpline->SetTimePerSegment(timePerSegment);
		}
	}
}
