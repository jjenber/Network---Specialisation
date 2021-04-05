#pragma once
#include "GameDefines.h"
#include "Entt\entt.hpp"
#include <CommonUtilities\Math\Vector3.h>

class Game
{
public:
	entt::entity InstantiateEntity();
	entt::entity InstantiateEntity(const CommonUtilities::Vector3f& aPosition);

	size_t GetEntityCount() const;
	void SetUniqueID(const entt::id_type aLocalID, const entt::id_type aUniqueID);

	const entt::registry& GetRegistry() { return myRegistry; };
protected:
	entt::registry myRegistry;
};

