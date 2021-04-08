#include "pch.h"
#include "Client.h"
#include "Timer\Timer.h"

#include <iostream>
#include <chrono>

Network::Client::Client() : 
	myWorldServerConnection(myWorldServerSocket), 
	myAreaServerConnection(myAreaServerSocket)
{}

void Network::Client::Init()
{
	myWorldServerSocket.SetBlocking(false);
	myWorldServerAddress = Address("127.0.0.1", Constants::WORLD_TO_CLIENT_PORT);

	if (myWorldServerConnection.Connect(myWorldServerAddress, 10.f, eNETMESSAGE_CLIENT_HANDSHAKE))
	{
		std::cout << "Connected" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect to World Server" << std::endl;
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

	if (myAreaServerConnection.GetConnectionStatus() != eConnectionStatus::Disconnected)
	{
		myAreaServerConnection.Update(aDeltatime);
		while (myAreaServerConnection.HasMessages())
		{
			HandleAreaServerMessages();
		}
	}
}

void Network::Client::HandleWorldServerMessages()
{
	using namespace Network;
	MessageID_t id = myWorldServerConnection.Peek();

	switch (id)
	{
	case eNETMESSAGE_R_CLIENT_ENTER_AREA:
	{
		ClientEnterAreaMessage msg;
		myWorldServerConnection.ReadNextMessage(msg);
		
		myUniqueID = msg.myUniqueID;
		myPosition = CommonUtilities::Vector3f
		{ 
			static_cast<float>(msg.myPosition.x), 
			static_cast<float>(msg.myPosition.y), 
			static_cast<float>(msg.myPosition.z) 
		};
		myAreaServerAddress = Address(msg.myAreaServerIP, msg.myAreaServerPort);
		myAreaServerConnection.Connect(myAreaServerAddress, 10.f, eNETMESSAGE_CLIENT_HANDSHAKE);

		break;
	}
	default:
		std::cout << "Unhandled message id: " << (int)id << std::endl;
		break;
	}

}

void Network::Client::HandleAreaServerMessages()
{
	using namespace Network;
	MessageID_t id = myWorldServerConnection.Peek();

	switch (id)
	{
		

	}
}
