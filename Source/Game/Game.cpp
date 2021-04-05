#include "pch.h"
#include "Game.h"
#include "Components\Transform.hpp"
#include "Components\UniqueID.hpp"

entt::entity Game::InstantiateEntity()
{
	return InstantiateEntity(cu::Vector3f());
}

entt::entity Game::InstantiateEntity(const cu::Vector3f& aPosition)
{
	entt::entity entity = myRegistry.create();
	myRegistry.emplace<components::Transform>(entity).myPosition = aPosition;
	return entity;
}

size_t Game::GetEntityCount() const
{
	return myRegistry.size<components::UniqueID>();
}

void Game::SetUniqueID(const entt::id_type aLocalID, const entt::id_type aUniqueID)
{
	entt::entity entity = entt::entity(aLocalID);
	if (myRegistry.valid(entity))
	{
		myRegistry.emplace_or_replace<components::UniqueID>(entity, aUniqueID);
	}
}
