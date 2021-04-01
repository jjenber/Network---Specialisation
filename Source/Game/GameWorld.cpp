#include "pch.h"
#include "GameWorld.h"

void GameWorld::Init()
{

}

void GameWorld::InstantiateEntities(const int aEntityCount, std::vector<entt::entity>& aUniqueIDs)
{
	aUniqueIDs.reserve(aEntityCount);
	for (int i = 0; i < aEntityCount; i++)
	{
		aUniqueIDs.push_back(myWorldRegistry.create());
	}
}

int GameWorld::GetUnassignedRegionIndex() const
{
	for (int i = 0; i < REGION_COUNT; i++)
	{
		if (myRegions[i].myAreaServerSlot == -1)
		{
			return i;
		}
	}
	return -1;
}
