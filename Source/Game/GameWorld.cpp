#include "pch.h"
#include "GameWorld.h"
#include "Components\Transform.hpp"
#include "Components\Client.hpp"

void GameWorld::Init() {}

void GameWorld::InstantiateEntities(const int aEntityCount, std::vector<entt::entity>& aUniqueIDs)
{
	aUniqueIDs.reserve(aEntityCount);

	for (int i = 0; i < aEntityCount; i++)
	{
		entt::entity entity = myWorldRegistry.create();
		aUniqueIDs.push_back(entity);
		myWorldRegistry.emplace<components::Transform>(entity);
	}
}

entt::entity GameWorld::InstantiateClient(const CommonUtilities::Vector3f& aPosition)
{
	entt::entity entity = myWorldRegistry.create();

	myWorldRegistry.emplace<components::Client>(entity);
	myWorldRegistry.emplace<components::Transform>(entity);

	return entity;
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

void GameWorld::InitRegion(unsigned int aRegionID, unsigned int aAreaServerID)
{
	myRegions[aRegionID].myAreaServerSlot = aAreaServerID;
}

void GameWorld::UpdateEntityState(entt::id_type aID, float myX, float myZ)
{
	components::Transform& transform = myWorldRegistry.get<components::Transform>(entt::entity(aID));

	transform.myPosition.x = myX;
	transform.myPosition.z = myZ;
}

CommonUtilities::Vector3f GameWorld::GetPosition(entt::entity aEntity) const
{
	return 
		myWorldRegistry.valid(aEntity) ?
		myWorldRegistry.get<components::Transform>(aEntity).myPosition :
		CommonUtilities::Vector3f();
};

void GameWorld::DestroyEntity(entt::entity aEntity)
{
	if (myWorldRegistry.valid(aEntity))
	{
		myWorldRegistry.destroy(aEntity);
	}
}

