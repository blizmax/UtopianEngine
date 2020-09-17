#pragma once
#include <functional>
#include <string>
#include "vulkan/VulkanApp.h"
#include "utility/Module.h"
#include "utility/Common.h"

namespace Utopian
{
	class Engine : public Module<Engine>
	{
	public:
		Engine(Window* window, const std::string& appName);
		~Engine();

		/** Executes the main loop and calls Engine::Tick() every frame. */
		void Run();

		/** Handles Win32 messages and forwards them to needed modules. */
		void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		/** Registers a callback function to be called in Engine::Update(). */
		template<class ...Args>
		void RegisterUpdateCallback(Args &&...args)
		{
			mUpdateCallback = std::bind(std::forward<Args>(args)...);
		}

		/** Registers a callback function to be called in Engine::Render(). */
		template<class ...Args>
		void RegisterRenderCallback(Args &&...args)
		{
			mRenderCallback = std::bind(std::forward<Args>(args)...);
		}

		/** Registers a callback function to be called in Engine::~Engine(). */
		template<class ...Args>
		void RegisterDestroyCallback(Args &&...args)
		{
			mDestroyCallback = std::bind(std::forward<Args>(args)...);
		}

	private:
		/**
		 * Calls the per frame update function of all modules in the engine.
		 * 
		 * @note Also calls registered application callback functions.
		 */
		void Tick();

		/** Starts all the modules included in the engine. */
		void StartModules();

		/** Updates all modules included in the engine. */
		void Update();

		/** Renders all modules included in the engine. */
		void Render();

	private:
		SharedPtr<Vk::VulkanApp> mVulkanApp;
		Window* mWindow;
		std::function<void()> mUpdateCallback;
		std::function<void()> mRenderCallback;
		std::function<void()> mDestroyCallback;
		std::string mAppName;
	};

	/** Returns an instance to the Engine module. */
	Engine& gEngine();
}
