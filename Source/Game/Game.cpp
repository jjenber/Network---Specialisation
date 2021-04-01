#include "pch.h"
#include "Game.h"
#include "Components\Transform.hpp"
#include "Components\Network.hpp"

void Game::InstantiateEntity()
{
	entt::entity entity = myRegistry.create();
	myRegistry.emplace<components::Transform>(entity);
	myRegistry.emplace<components::Network>(entity);
}

int Game::GetEntityCount() const
{
	return myRegistry.size<components::Network>();
}

void Game::SetUniqueID(const entt::id_type aLocalID, const entt::id_type aUniqueID)
{
	entt::entity entity = entt::entity(aLocalID);
	if (myRegistry.valid(entity))
	{
		myRegistry.emplace_or_replace<components::Network>(entity, aUniqueID);
	}
}
