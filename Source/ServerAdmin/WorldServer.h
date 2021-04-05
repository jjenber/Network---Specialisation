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
	std::vector<entt::entity> myEntities;
};

class WorldServer
{
public:
	void Startup();

	void InstantiateAreaServers();

	void Update(const float aDeltatime);

	const std::array<AreaServerInstance, MAX_AREA_SERVERS>& GetAreaServerInstanceArray() const { return myAreaServerInstances; }
	const entt::registry& GetRegistry() const { return myGameWorld.GetRegistry(); }
	bool CanStartAreaServer() const;
private:
	void HandleAreaServerMessages();
	void OnAreaServerConnected(int aAreaServerID, const Network::Address& aAddress);

	void DeployAreaServer(unsigned short aAreaServerID);

	void SendRequestEntityStateRequests(const float aDeltatime);

	std::array<AreaServerInstance, MAX_AREA_SERVERS> myAreaServerInstances;

	Network::MultiConnection myConnection;
	double myTime = 0;

	GameWorld myGameWorld;
};

