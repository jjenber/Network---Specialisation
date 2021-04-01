#include "pch.h"
#include "GameArea.h"
#include "Components\Network.hpp"

void GameArea::Init(int aRegionID)
{
	myRegion = aRegionID;
	
	for (int i = 0; i < 10; i++)
	{
		InstantiateEntity();
	}
}

std::vector<entt::id_type> GameArea::GetLocalIDs() const
{
	std::vector<entt::id_type> result;
	result.reserve(myRegistry.size<components::Network>());
	auto view = std::as_const(myRegistry).view<const components::Network>();

	for (auto&& [id, net] : view.each())
	{
		result.push_back(entt::id_type(id));
	}

	return result;
}
