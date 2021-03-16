#include "pch.h"
#include "Client.h"
#include "NetMessage.h"
#include <iostream>
#include <chrono>

void Network::Client::Init()
{
	myUDPSocket.SetBlocking(false);
	myMainServerAddress = Address("127.0.0.1", Constants::DEFAULT_PORT);
}

void Network::Client::Update()
{
	myReliableMessageQueue.Send(myUDPSocket);
}

void Network::Client::ConnectToServer()
{
	ReliableNetMessage msg(eNETMESSAGE_HANDSHAKE);
	myReliableMessageQueue.Enqueue(msg, myMainServerAddress);
}
