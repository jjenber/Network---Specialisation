#include "pch.h"
#include "GameArea.h"

#include <CommonUtilities\Random\Random.h>

#include "Components\UniqueID.hpp"
#include "Components\Transform.hpp"
#include "Components\Velocity.hpp"
#include "Components\Client.hpp"

void GameArea::Init(int aRegionID)
{
	Random::SetSeed(aRegionID);

	myRegion = aRegionID;
	myRegistry.prepare<components::UniqueID>();

	for (int i = 0; i < Random::Range(100, 200); i++)
	{
		const float rX = Random::Range(0.f, REGION_SIZEF);
		const float rZ = Random::Range(0.f, REGION_SIZEF);
		entt::entity entity = InstantiateEntity(cu::Vector3f(rX, 0, rZ));
		if (i % 10 == 0)
		{
			myRegistry.emplace<components::Velocity>(entity, CommonUtilities::Vector3f(rX, 0, rZ).GetNormalized() * 20.f);
		}
	}
}

std::vector<entt::id_type> GameArea::GetUnsyncedIDs() const
{
	std::vector<entt::id_type> result;
	
	for (auto&& [id, transform] : myRegistry.view<const components::Transform>(entt::exclude<components::UniqueID>).each())
	{
		result.push_back(entt::id_type(id));
	}

	return result;
}

std::vector<entt::id_type> GameArea::GetUniqueIDs() const
{
	std::vector<entt::id_type> result;
	result.reserve(myRegistry.size<components::UniqueID>());

	for (auto&& [id, net] : std::as_const(myRegistry).view<const components::UniqueID>().each())
	{
		result.push_back(entt::id_type(id));
	}
	return result;
}

void GameArea::SetClientVelocity(entt::entity aLocalID, const CommonUtilities::Vector3f& aVelocity)
{
	myRegistry.get<components::Velocity>(entt::entity(aLocalID)).myVelocity = aVelocity;
}

void GameArea::Update(const float aDeltatime)
{
	// Clients
	for (auto&& [entity, transform, vel] : myRegistry.view<components::Transform, components::Velocity, components::Client>().each())
	{
		transform.myPosition += vel.myVelocity * aDeltatime;
	}

	// NPCs
	const float maxSpeed = 100.f;
	for (auto&& [entity, transform, vel] : myRegistry.view<components::Transform, components::Velocity>(entt::exclude<components::Client>).each())
	{
		transform.myPosition += vel.myVelocity * aDeltatime;
		vel.myVelocity.x += Random::Range(-10.f, 10.f);
		vel.myVelocity.z += Random::Range(-10.f, 10.f);
		if (vel.myVelocity.Length() > maxSpeed)
		{
			vel.myVelocity.Normalize();
			vel.myVelocity *= maxSpeed;
		}

		if (transform.myPosition.x > REGION_SIZE)
			transform.myPosition.x = REGION_SIZE;
		if (transform.myPosition.x < 0.f)
			transform.myPosition.x = 0.f;
		if (transform.myPosition.z > REGION_SIZE)
			transform.myPosition.z = REGION_SIZE;
		if (transform.myPosition.z < 0.f)
			transform.myPosition.z = 0.f;
	}
}
