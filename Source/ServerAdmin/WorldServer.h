#pragma once
#include "NetMessage\NetMessageQueue.hpp"
#include "NetMessage\ReliableNetMessageQueue.h"
#include "Connection\MultiConnection.h"
#include "../AreaServer/AreaServerStatus.h"
#include "../Game/GameWorld.h"
#include <bitset>

constexpr auto MAX_AREA_SERVERS = 8;

struct AreaServerInstance
{
	Network::Address myAddress;
	eAreaServerStatus myStatus = eAreaServerStatus::Shutdown;
	std::vector<int> myRegions;
};

class WorldServer
{
public:
	void Startup();

	void InstantiateAreaServers();

	void Update(const float aDeltatime);

private:
	void HandleAreaServerMessages();

	std::array<AreaServerInstance, MAX_AREA_SERVERS> myInstances;
	Network::MultiConnection myConnection;
	Network::NetMessageQueue<1024> myAreaServerMessages;
	double myTime = 0;

	GameWorld myGameWorld;
};

