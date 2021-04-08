#include "pch.h"
#include "Game.h"
#include "Components\Transform.hpp"
#include "Components\UniqueID.hpp"
#include "Components\Client.hpp"
#include "Components\Velocity.hpp"

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
	myRegistry.emplace<components::Velocity>(entity);
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

CommonUtilities::Vector3f Game::GetPosition(entt::entity aID) const
{
	if (myRegistry.valid(aID))
	{
		return myRegistry.get<components::Transform>(aID).myPosition;
	}
	return CommonUtilities::Vector3f();
}

uint32_t Game::GetUniqueID(entt::entity aID) const
{
	if (myRegistry.valid(aID))
	{
		return myRegistry.get<components::UniqueID>(aID).myUniqueID;
	}
	return UINT32_MAX;
}

void Game::SetUniqueID(const entt::id_type aLocalID, const entt::id_type aUniqueID)
{
	entt::entity entity = entt::entity(aLocalID);
	if (myRegistry.valid(entity))
	{
		myRegistry.emplace_or_replace<components::UniqueID>(entity, aUniqueID);
	}
}
