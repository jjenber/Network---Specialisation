#include "pch.h"
#include "Game.h"
#include "Components\Transform.hpp"
#include "Components\UniqueID.hpp"
#include "Components\Client.hpp"

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

entt::entity Game::InstantiateClient(const CommonUtilities::Vector3f& aPosition, entt::id_type aUniqueID)
{
	entt::entity entity = InstantiateEntity(aPosition);
	myRegistry.emplace<components::Client>(entity);
	myRegistry.emplace<components::UniqueID>(entity, aUniqueID);
	return entity;
}

entt::entity Game::InstantiateClient(const CommonUtilities::Vector3<uint16_t>& aPosition, entt::id_type aUniqueID)
{
	CommonUtilities::Vector3f pos{ static_cast<float>(aPosition.x),static_cast<float>(aPosition.y),static_cast<float>(aPosition.z) };
	return InstantiateClient(pos, aUniqueID);
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
