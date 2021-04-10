#pragma once
#include "GameDefines.h"
#include "Entt\entt.hpp"
#include <CommonUtilities\Math\Vector3.h>

class Game
{
public:
	entt::entity InstantiateEntity();
	entt::entity InstantiateEntity(const CommonUtilities::Vector3f& aPosition);
	entt::entity InstantiateClient(entt::id_type aUniqueID, const CommonUtilities::Vector3f& aPosition, const CommonUtilities::Vector3f& aVelocity);

	size_t GetEntityCount() const;
	void SetUniqueID(const entt::id_type aLocalID, const entt::id_type aUniqueID);

	CommonUtilities::Vector3f GetPosition(entt::entity aID) const;
	CommonUtilities::Vector3f GetVelocity(entt::entity aID) const;

	uint32_t GetUniqueID(entt::entity aID) const;

	entt::registry& GetRegistry() { return myRegistry; };

protected:
	entt::registry myRegistry;
};
