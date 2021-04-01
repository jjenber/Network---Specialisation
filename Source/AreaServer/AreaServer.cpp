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
	myWorldServerConnection.Update();

	while (myWorldServerConnection.HasMessages())
	{
		ReadWorldServerMessage(myWorldServerConnection.Peek());
	}

	myWorldServerConnection.ClearMessages();
	return myIsRunning;
}


void AreaServer::ReadWorldServerMessage(Network::MessageID_t aMessageID)
{
	switch (aMessageID)
	{
	case Network::eNETMESSAGE_R_AS_DEPLOY:
	{
		Network::DeployAreaServer msg;
		myWorldServerConnection.ReadNextMessage(msg);
		
		myGame.Init(msg.myRegionID);

		myStatus = eAreaServerStatus::Running;

		Network::AreaServerStatus statusMsg((unsigned char)myStatus);
		myWorldServerConnection.Send(statusMsg);

		SendIDRequests();
	
	}	break;

	case Network::eNETMESSAGE_R_AS_REQUEST_IDS_RESPONSE:
	{
		Network::ResponseUniqueIDs response(0);
		myWorldServerConnection.ReadNextMessage(response);
		
		std::cout << "Received " << (int)response.myCount << " ids." << std::endl;

		for (int i = 0; i < response.myCount * 2; i += 2)
		{
			myGame.SetUniqueID(response.myMappedIDs[i], response.myMappedIDs[i + 1]);
		}

	}	break;

	case Network::eNETMESSAGE_R_AS_REQUEST_ENTITY_STATES:
	{
		Network::ReliableNetMessage msg;
		myWorldServerConnection.ReadNextMessage(msg);
		std::cout << "received request for entities " << std::endl;
	}	break;
	default:
		std::cout << "Message ID: " << (int)aMessageID << " not handled." << std::endl;
		break;
	}
}

void AreaServer::SendIDRequests()
{
	const std::vector<entt::id_type> ids = myGame.GetLocalIDs();

	Network::RequestUniqueIDs requestIDsMsg;
	int index = 0;
	while (index < ids.size())
	{
		int count = 0;
		for (int itr = index; itr < ids.size(); itr++, count++)
		{
			int index = itr % Network::RequestUniqueIDs::MaxCount;
			requestIDsMsg.myLocalIDs[index] = ids[itr];
		}
		index += count;
		requestIDsMsg.SetCount(static_cast<uint8_t>(count));
		myWorldServerConnection.Send(requestIDsMsg);
		std::cout << "Sending req for " << count << std::endl;
	}
}
