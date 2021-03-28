#include "AreaServer.h"
#include "NetMessage\NetMessage.h"
#include "Timer\Timer.h"
#include <iostream>

AreaServer::AreaServer() : 
	myWorldServerConnection(mySocket),
	myConnectionStatus(Network::eConnectionStatus::Disconnected),
	myIsRunning(false),
	myStatus(eAreaServerStatus::Shutdown)
{
}

bool AreaServer::Startup()
{
	myWorldServerAddress = Network::Address("127.0.0.1", Network::Constants::DEFAULT_PORT);
	myIsRunning = myWorldServerConnection.Connect(myWorldServerAddress, 3.f, Network::eNETMESSAGE_AS_HANDSHAKE);
	return myIsRunning;
}


bool AreaServer::Update(const float aDeltatime)
{
	return myIsRunning;
}