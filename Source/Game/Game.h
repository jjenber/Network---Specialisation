#pragma once
#include "GameDefines.h"
#include "Entt\entt.hpp"
#include <CommonUtilities\Math\Vector3.h>

class Game
{
public:
	entt::entity InstantiateEntity();
	entt::entity InstantiateEntity(const CommonUtilities::Vector3f& aPosition);
	entt::entity InstantiateClient(const CommonUtilities::Vector3f& aPosition, entt::id_type aUniqueID);
	entt::entity InstantiateClient(const CommonUtilities::Vector3<uint16_t>& aPosition, entt::id_type aUniqueID);

	size_t GetEntityCount() const;
	void SetUniqueID(const entt::id_type aLocalID, const entt::id_type aUniqueID);

	CommonUtilities::Vector3f GetPosition(entt::entity aID) const;
	uint32_t GetUniqueID(entt::entity aID) const;

	const entt::registry& GetRegistry() { return myRegistry; };

protected:
	entt::registry myRegistry;
};
