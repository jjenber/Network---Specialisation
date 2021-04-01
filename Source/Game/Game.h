#pragma once
#include "Entt\entt.hpp"
class Game
{
public:
	void InstantiateEntity();
	int GetEntityCount() const;
	void SetUniqueID(const entt::id_type aLocalID, const entt::id_type aUniqueID);
protected:
	entt::registry myRegistry;
};

