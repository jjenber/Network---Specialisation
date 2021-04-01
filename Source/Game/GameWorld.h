#pragma once
#include "GameDefines.h"
#include "Entt\entt.hpp"
#include <vector>

class GameWorld
{
	struct Region
	{
		int myAreaServerSlot = -1;
	};

public:
	void Init();
	void InstantiateEntities(const int aEntityCount, std::vector<entt::entity>& aUniqueIDs);
	int GetUnassignedRegionIndex() const;

private:
	entt::registry myWorldRegistry;
	std::array<Region, REGION_COUNT> myRegions;
};

