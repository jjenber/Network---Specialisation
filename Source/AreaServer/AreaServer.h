#pragma once
#include "Common.h"
#include "NetMessage\NetMessageQueue.hpp"

enum class eAreaServerStatus
{
	Idling,
	Loading,
	Running,
};

class AreaServer
{
public:
	void Startup();
	Network::eConnectionStatus ConnectToWorldServer();
	bool Update();

private:
	uint8_t myServerID = UINT8_MAX;

	// World Server
	Network::Address myWorldServerAddress;
	Network::UDPSocket myWorldServerSocket;
	Network::eConnectionStatus myConnectionStatus;
	Network::NetMessageQueue<1024> myMessageQueue;

	// Clients

	eAreaServerStatus myStatus;
	bool myIsRunning;
};
