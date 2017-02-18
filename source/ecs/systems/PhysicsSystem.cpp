#include "ecs/components/PhysicsComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/Entity.h"
#include "PhysicsSystem.h"

namespace ECS
{
	PhysicsSystem::PhysicsSystem(EntityManager* entityManager)
		: System(entityManager, Type::PHYSICS_COMPONENT)
	{
	}

	void PhysicsSystem::AddEntity(Entity* entity)
	{
		EntityCache entityCache;
		entityCache.entity = entity;
		entityCache.transform = dynamic_cast<TransformComponent*>(entity->GetComponent(ECS::Type::TRANSFORM_COMPONENT));
		entityCache.physics = dynamic_cast<PhysicsComponent*>(entity->GetComponent(ECS::Type::PHYSICS_COMPONENT));

		mEntities.push_back(entityCache);
	}

	void PhysicsSystem::RemoveEntity(Entity* entity)
	{
		for (auto iter = mEntities.begin(); iter < mEntities.end(); iter++)
		{
			if ((*iter).entity->GetId() == entity->GetId())
			{
				iter = mEntities.erase(iter);
				break;
			}
		}
	}

	void PhysicsSystem::Process()
	{
		for (int i = 0; i < mEntities.size(); i++)
		{
			TransformComponent* transform = mEntities[i].transform;
			PhysicsComponent* physicsComponent = mEntities[i].physics;

			// Apply physics rules
			if (fabs(transform->GetPosition().x) > 1000.0f)
				physicsComponent->mVelocity.x *= -1;
			if (fabs(transform->GetPosition().y) > 1000.0f)
				physicsComponent->mVelocity.y *= -1;
			if (fabs(transform->GetPosition().z) > 1000.0f)
				physicsComponent->mVelocity.z *= -1;

			// Update the transform component
			transform->SetPosition(transform->GetPosition() + physicsComponent->GetVelocity());
			transform->SetRotation(transform->GetRotation() + physicsComponent->GetRotationSpeed());
			transform->SetScale(transform->GetScale() + physicsComponent->GetScaleSpeed());
		}
	}

	void PhysicsSystem::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

	}

	bool PhysicsSystem::Contains(Entity* entity)
	{
		for (EntityCache entityCache : mEntities)
		{
			if (entityCache.entity->GetId() == entity->GetId())
				return true;
		}

		return false;
	}
}