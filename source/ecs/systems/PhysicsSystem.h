#pragma once

#include <vector>
#include <window.h>
#include "System.h"

namespace VulkanLib
{
}

namespace ECS
{
	class TransformComponent;
	class PhysicsComponent;


	class PhysicsSystem : public System
	{
		struct EntityCache
		{
			Entity* entity;
			TransformComponent* transform;
			PhysicsComponent* physics;
		};

	public:
		PhysicsSystem(EntityManager* entityManager);
		void AddEntity(Entity* entity);
		void RemoveEntity(Entity* entity);
		void Process();

		virtual void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool Contains(Entity* entity);
	private:
		std::vector<EntityCache> mEntities;
	};
}