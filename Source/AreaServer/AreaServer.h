#pragma once
#include "Common.h"
#include "NetMessage\NetMessageQueue.hpp"
#include "NetMessage\ReliableNetMessageQueue.h"
#include "Connection\UnaryConnection.h"

enum class eAreaServerStatus
{
	Shutdown,
	Idling,
	Loading,
	Running,
};

class AreaServer
{
public:
	AreaServer();
	bool Startup();
	bool Update(const float aDeltatime);

	inline Network::eConnectionStatus GetStatus() const { return myConnectionStatus; }
private:

	uint8_t myServerID = UINT8_MAX;
	Network::UDPSocket mySocket;

	// World Server
	Network::UnaryConnection myWorldServerConnection;
	Network::Address myWorldServerAddress;
	Network::eConnectionStatus myConnectionStatus;

	// Clients
	eAreaServerStatus myStatus;
	bool myIsRunning;
};
