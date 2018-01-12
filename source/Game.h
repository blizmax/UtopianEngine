#pragma once

#include "vulkan/VulkanInclude.h"
#include "Platform.h"
#include "Timer.h"
#include "Common.h"

namespace ECS
{
	class SystemManager;
	class Entity;
}

class Terrain;
class Input;

namespace Scene
{
	class SceneRenderer;
}

namespace Vulkan
{
	class Game
	{
	public:
		Game(Window* window);
		~Game();

		void RenderLoop();
		void Update();
		void Draw();

		virtual void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		void InitScene();	
		void InitTestScene();	
		bool IsClosing();

		// Move all of these to other locations
		SharedPtr<Renderer> mRenderer;
		SharedPtr<Camera> mCamera;
		SharedPtr<Terrain> mTerrain;
		SharedPtr<Input>  mInput;
		Window* mWindow;
		Timer mTimer;
		std::string mTestCaseName;
		bool mIsClosing;

		ECS::SystemManager* mEntityManager;
		ECS::Entity* mTestEntity;
	};
}
