#include "pch.h"
#include "Game.h"
#include "Components\Transform.hpp"
#include "Components\UniqueID.hpp"
#include "Components\Client.hpp"
#include "Components\Velocity.hpp"
#include "Components\ShadowClient.hpp"

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

entt::entity Game::InstantiateClient(entt::id_type aUniqueID, const CommonUtilities::Vector3f& aPosition, const CommonUtilities::Vector3f& aVelocity)
{
	entt::entity entity = InstantiateEntity(aPosition);
	myRegistry.emplace<components::Client>(entity);
	myRegistry.emplace<components::UniqueID>(entity, aUniqueID);
	myRegistry.emplace<components::Velocity>(entity, aVelocity);
	return entity;
}

entt::entity Game::InstantiateShadowClient(entt::id_type aUniqueID, const CommonUtilities::Vector3f& aPosition, const CommonUtilities::Vector3f& aVelocity)
{
	entt::entity entity = InstantiateClient(aUniqueID, aPosition, aVelocity);
	myRegistry.emplace<components::ShadowClient>(entity);
	return entity;
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

CommonUtilities::Vector3f Game::GetVelocity(entt::entity aID) const
{
	if (myRegistry.valid(aID))
	{
		return myRegistry.get<components::Velocity>(aID).myVelocity;
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
