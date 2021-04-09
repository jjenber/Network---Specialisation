#pragma once
#include "Game.h"

#include "Entt\entt.hpp"
#include <vector>

class GameArea : public Game
{
public:
	void Init(int aRegionID);

	void Update(const float aDeltatime);

	int GetRegion() const { return myRegion; }
	std::vector<entt::id_type> GetUnsyncedIDs() const;
	std::vector<entt::id_type> GetUniqueIDs() const;

	int ChangeClientIntoShadowEntity(entt::id_type aUniqueID);
	void SetClientVelocity(entt::entity aUniqueID, const CommonUtilities::Vector3f& aVelocity);
private:
	int myRegion = -1;
};

