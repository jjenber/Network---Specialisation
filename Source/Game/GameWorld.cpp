#include "pch.h"
#include "GameWorld.h"

void GameWorld::Init()
{
	for (int i = 0; i < myRegions.size(); i++)
	{
		myRegions[i].myIndex = i;
	}
}

void GameWorld::InstantiateEntities(const int aEntityCount, std::vector<entt::entity>& aUniqueIDs)
{
	aUniqueIDs.reserve(aEntityCount);
	for (int i = 0; i < aEntityCount; i++)
	{
		aUniqueIDs.push_back(myWorldRegistry.create());
	}
}
