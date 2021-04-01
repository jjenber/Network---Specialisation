#pragma once
#include "Game.h"
#include "Entt\entt.hpp"
#include <vector>
class GameArea : public Game
{
public:
	void Init(int aRegionID);

	int GetRegion() const { return myRegion; }
	std::vector<entt::id_type> GetLocalIDs() const;
private:
	int myRegion = -1;
};

