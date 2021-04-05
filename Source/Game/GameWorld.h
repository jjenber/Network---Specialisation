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
	void InitRegion(unsigned int aRegionID, unsigned int aAreaServerID);
	void UpdateEntityState(entt::id_type aID, float myX, float myZ);
	const entt::registry& GetRegistry() const { return myWorldRegistry; };

private:
	entt::registry myWorldRegistry;
	std::array<Region, REGION_COUNT> myRegions;
};

