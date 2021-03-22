#pragma once
#include "Common.h"
#include "NetMessage\NetMessageQueue.hpp"

class AreaServer
{
public:
	void Startup();
	Network::eConnectionStatus ConnectToWorldServer();
private:

	Network::Address myWorldServerAddress;
	Network::UDPSocket myWorldServerSocket;
	Network::eConnectionStatus myConnectionStatus;
	Network::NetMessageQueue<1024> myMessageQueue;
	uint8_t myServerID = UINT8_MAX;
};
