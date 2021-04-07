#include "pch.h"
#include "Client.h"
#include "Timer\Timer.h"

#include <iostream>
#include <chrono>

Network::Client::Client() : myWorldServerConnection(myUDPSocket)
{}

void Network::Client::Init()
{
	myUDPSocket.SetBlocking(false);
	myWorldServerAddress = Address("127.0.0.1", Constants::WORLD_TO_CLIENT_PORT);

	if (myWorldServerConnection.Connect(myWorldServerAddress, 20.f, eNETMESSAGE_CLIENT_HANDSHAKE))
	{

	}
}

void Network::Client::Disconnect()
{
	ReliableNetMessage disconnect(eNETMESSAGE_R_DISCONNECT);
	disconnect.mySenderID = myClientSlot;
	
	myWorldServerConnection.Send(disconnect);
}

void Network::Client::Update(float aDeltatime)
{
	myWorldServerConnection.Update(aDeltatime);
	
	while (myWorldServerConnection.HasMessages())
	{
		HandleWorldServerMessages();
	}
}

void Network::Client::HandleWorldServerMessages()
{
	using namespace Network;
	MessageID_t id = myWorldServerConnection.Peek();

	switch (id)
	{
	case eNETMESSAGE_R_CLIENT_SPAWN:
	{
		ReliableNetMessage msg;
		myWorldServerConnection.ReadNextMessage(msg);
		std::cout << "Spawn received from WS" << std::endl;
		break;
	}
	default:
		std::cout << "Unhandled message id: " << (int)id << std::endl;
		break;
	}

}
