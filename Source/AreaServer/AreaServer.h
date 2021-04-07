#pragma once
#include "Common.h"
#include "Connection\MultiConnection.h"
#include "Connection\UnaryConnection.h"
#include "AreaServerStatus.h"
#include "../Game/GameArea.h"

class AreaServer
{
public:
	AreaServer();
	
	bool Startup();

	bool Update(const float aDeltatime);

	inline Network::eConnectionStatus GetStatus() const { return myConnectionStatus; }
private:
	void ReadWorldServerMessage(Network::MessageID_t aMessageID);

	void SendIDRequests();
	void SendEntityStates();

	uint8_t myServerID = UINT8_MAX;

	// Game
	GameArea myGame;

	// Clients
	Network::MultiConnection myClientConnections;

	// World Server
	Network::UDPSocket mySocket;
	Network::UnaryConnection myWorldServerConnection;
	Network::Address myWorldServerAddress;
	Network::eConnectionStatus myConnectionStatus;

	eAreaServerStatus myStatus;
	bool myIsRunning;
};
