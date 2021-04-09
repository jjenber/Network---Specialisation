#include "pch.h"
#include "Client.h"
#include "Timer\Timer.h"

#include <CommonUtilities\Random\Random.h>
#include <iostream>
#include <chrono>

//////////////////////////////////////////////////////////////////////////////
/// 
///  The current purpose of this class is just to simulate the connection to
///  a World Server from a user, and to seamlessly travel between dedicated 
///  Area Servers. No actual gameplay is done for the client.
/// 
///////////////////////////////////////////////////////////////////////////////

Network::Client::Client()
{}

void Network::Client::Init()
{
	myVelocity.x = Random::Range(-1.f, 1.f);
	myVelocity = myVelocity.GetNormalized() * 100.f;

	myWorldServerConnection.Init(myWorldServerSocket);
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
	mySendMoveTimer += aDeltatime;

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

		if (mySendMoveTimer > 0.4)
		{
			ClientMoveMessage move(myUniqueID, myVelocity);
			myAreaServerConnection.Send(move);
			mySendMoveTimer = 0.f;
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
		myAreaServerConnection.Disconnect();
		myAreaServerSocket = Network::UDPSocket();
		myAreaServerConnection.Init(myAreaServerSocket);

		if (!myAreaServerConnection.Connect(myAreaServerAddress, 10.f, eNETMESSAGE_CLIENT_HANDSHAKE))
		{
			std::cout << "Timed out " << std::endl;
		}

		ClientValidateTokenMessage validate;
		validate.myToken    = msg.myToken;
		validate.myUniqueID = msg.myUniqueID;
		myAreaServerConnection.Send(validate);
		break;
	}

	default:
		std::cout << "Unhandled message from world server: " << (int)id << std::endl;
		break;
	}

}

void Network::Client::HandleAreaServerMessages()
{
	using namespace Network;
	MessageID_t id = myAreaServerConnection.Peek();

	switch (id)
	{
		case eNETMESSAGE_CLIENT_MOVE:
		{
			ClientServerPosition msg;
			myAreaServerConnection.ReadNextMessage(msg);
			myPosition = msg.myPosition;
			break;
		}
		default:
			std::cout << "Unhandled message from Area Server " << (int)id << std::endl;
			break;
	}
}
