#include "pch.h"
#include "GameArea.h"

#include <CommonUtilities\Random\Random.h>

#include "Components\UniqueID.hpp"
#include "Components\Transform.hpp"
#include "Components\Velocity.hpp"

void GameArea::Init(int aRegionID)
{
	Random::SetSeed(aRegionID);

	myRegion = aRegionID;
	myRegistry.prepare<components::UniqueID>();
	for (int z = 0; z < Random::Range(1, 8); z++)
	{
		for (int x = 0; x < Random::Range(2, 10); x ++)
		{
			const float rX = Random::Range(0.f, REGION_SIZEF);
			const float rZ = Random::Range(0.f, REGION_SIZEF);
			entt::entity entity = InstantiateEntity(cu::Vector3f(rX, 0, rZ));
			if (x % 5 == 0)
			{
				myRegistry.emplace<components::Velocity>(entity, CommonUtilities::Vector3f(rX, 0, rZ).GetNormalized() * 20.f);
			}
		}
	}
}

std::vector<entt::id_type> GameArea::GetUnsyncedIDs() const
{
	std::vector<entt::id_type> result;

	auto view = myRegistry.view<const components::Transform>(entt::exclude<components::UniqueID>);
	
	for (auto&& [id, transform] : view.each())
	{
		result.push_back(entt::id_type(id));
	}

	return result;
}

std::vector<entt::id_type> GameArea::GetUniqueIDs() const
{
	std::vector<entt::id_type> result;
	result.reserve(myRegistry.size<components::UniqueID>());
	auto view = std::as_const(myRegistry).view<const components::UniqueID>();

	for (auto&& [id, net] : view.each())
	{
		result.push_back(entt::id_type(id));
	}
	return result;
}

void GameArea::Update(const float aDeltatime)
{
	const float maxSpeed = 100.f;
	for (auto&& [entity, transform, vel] : myRegistry.view<components::Transform, components::Velocity>().each())
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
		if (transform.myPosition.y > REGION_SIZE)
			transform.myPosition.y = REGION_SIZE;
		if (transform.myPosition.y < 0.f)
			transform.myPosition.y = 0.f;
	}
}
