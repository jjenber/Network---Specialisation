#pragma once
#include "Common.h"
#include "Connection\MultiConnection.h"
#include "Connection\UnaryConnection.h"
#include "AreaServerStatus.h"
#include "../Game/GameArea.h"

#include <unordered_map>

struct ClientData
{
	Network::Address myAddress;
	entt::id_type myUniqueID;
	entt::entity myLocalID;
	double myLastMessageTime = 0.f;
	bool myIsConnected = false;
	bool myIsValidated = false;
	bool myIsMigrating = false;
	bool myIsShadow    = false;
};
struct QueueClientItem
{
	uint32_t myToken = 0;
	uint32_t myUniqueID = 0;
	CommonUtilities::Vector3f myPosition;
	CommonUtilities::Vector3f myVelocity;
};
namespace components
{
	struct MigrateClient;
}

class AreaServer
{
public:
	AreaServer();
	
	bool Startup();

	bool Update(const float aDeltatime);

	inline Network::eConnectionStatus GetStatus() const { return myConnectionStatus; }
private:
	void HandleWorldServerMessage(Network::MessageID_t aMessageID);
	void HandleClientMessage(Network::MessageID_t aMessageID);

	void SendIDRequests();
	void SendEntityStates();

	void OnClientConnected(const Network::Address& aAddress, uint16_t aConnectionSlot);

	void SyncClients(const float aDeltatime);
	void MigrateClient(ClientData& aClientData, const components::MigrateClient& aComponent);
	void ClientTimedOut(ClientData& aClientData);

	uint8_t myServerID = UINT8_MAX;

	// Game
	GameArea myGame;

	// Clients
	Network::MultiConnection myClientConnections;
	std::array<ClientData, MAX_CLIENT_COUNT> myClientData;
	std::vector<QueueClientItem> myQueuedClients;
	float mySyncClientsTimer = 0.f;

	// World Server
	Network::UDPSocket mySocket;
	Network::UnaryConnection myWorldServerConnection;
	Network::Address myWorldServerAddress;
	Network::eConnectionStatus myConnectionStatus;

	eAreaServerStatus myStatus;
	bool myIsRunning;
	double myTime = 0;
};
