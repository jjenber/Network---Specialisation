#pragma once

#include "Connection/MultiConnection.h"
#include "../AreaServer/AreaServerStatus.h"
#include "../Game/GameWorld.h"

constexpr auto MAX_AREA_SERVERS = REGION_COUNT;

struct AreaServerInstance
{
	Network::Address myAddress;
	Network::Address myClientConnectionAddress;
	eAreaServerStatus myStatus = eAreaServerStatus::Shutdown;
	std::vector<int> myRegions;
	std::vector<entt::entity> myEntities;
	std::vector<entt::entity> myClients;
	double myLastMessage = 0;
};

struct Client
{
	entt::entity myUniqueID;
	Network::Address myAddress;
	int myRegion = -1;
	int myNextRegion = -1;
	bool myIsMigrating = false;
};

class WorldServer
{
public:
	void Startup();

	void InstantiateAreaServers();

	void Update(const float aDeltatime);

	const std::array<AreaServerInstance, MAX_AREA_SERVERS>& GetAreaServerInstanceArray() const { return myAreaServerInstances; }
	
	const std::array<Client, MAX_CLIENT_COUNT>& GetClients() const { return myClients; };

	const entt::registry& GetRegistry() const { return myGameWorld.GetRegistry(); }
	
	bool CanStartAreaServer() const;

private:
	void DeployAreaServer(unsigned short aAreaServerID);
	void HandleAreaServerMessages();
	void OnAreaServerConnected(int aAreaServerID, const Network::Address& aAddress);
	void SendRequestEntityStateRequests(const float aDeltatime);
	
	void HandleClientMessages();
	void OnClientConnected(int aClientID, const Network::Address& aAddress);
	void HandleClientMigration(const Network::ClientMigrateMessage& aMessage, uint16_t aAreaServerConnectionID);
	

	std::array<AreaServerInstance, MAX_AREA_SERVERS> myAreaServerInstances;
	std::array<Client, MAX_CLIENT_COUNT> myClients;

	Network::MultiConnection myAreaServerConnection;
	Network::MultiConnection myClientConnections;
	
	double myTime = 0;
	GameWorld myGameWorld;
};

