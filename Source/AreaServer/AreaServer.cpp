#include "AreaServer.h"
#include "NetMessage\NetMessage.h"
#include "Timer\Timer.h"

#include "../Game/Components/Transform.hpp"
#include "../Game/Components/UniqueID.hpp"
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
	myWorldServerAddress = Network::Address("127.0.0.1", Network::Constants::WORLD_TO_AREA_PORT);
	myIsRunning = myWorldServerConnection.Connect(myWorldServerAddress, 3.f, Network::eNETMESSAGE_AS_HANDSHAKE);

	return myIsRunning;
}


bool AreaServer::Update(const float aDeltatime)
{
	myWorldServerConnection.Update(aDeltatime);

	while (myWorldServerConnection.HasMessages())
	{
		ReadWorldServerMessage(myWorldServerConnection.Peek());
	}

	myWorldServerConnection.ClearMessages();

	myGame.Update(aDeltatime);

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
		std::cout << "Sending status " << (int)statusMsg.myStatus << " from " << (int)statusMsg.mySenderID << std::endl;
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

	case Network::eNETMESSAGE_AS_REQUEST_ENTITY_STATES:
	{
		Network::NetMessage msg;
		myWorldServerConnection.ReadNextMessage(msg);
		std::cout << "received request for entities " << std::endl;
		SendEntityStates();
	}	break;
	default:
		std::cout << "Message ID: " << (int)aMessageID << " not handled." << std::endl;
		break;
	}
}

void AreaServer::SendIDRequests()
{
	const std::vector<entt::id_type> ids = myGame.GetUnsyncedIDs();
	Network::RequestUniqueIDs requestIDsMsg;

	int idIndex = 0;
	while (idIndex < ids.size())
	{
		int count = 0;
		for (int i = 0; i < Network::RequestUniqueIDs::MaxCount && idIndex < ids.size(); i++, idIndex++, count++)
		{
			requestIDsMsg.myLocalIDs[i] = ids[idIndex];
		}

		requestIDsMsg.SetCount(static_cast<uint8_t>(count));
		myWorldServerConnection.Send(requestIDsMsg);
		std::cout << "Sending req for " << count << std::endl;
	}
}

void AreaServer::SendEntityStates()
{
	using EntityState_t = Network::EntityState;

	std::vector<EntityState_t> entityStates;
	auto view = myGame.GetRegistry().view<const components::UniqueID, const components::Transform>();
	entityStates.reserve(view.size_hint());

	for (auto&& [localID, uniqueID, transform] : view.each())
	{
		entityStates.push_back(EntityState_t(transform.myPosition.x, transform.myPosition.z, uniqueID.myUniqueID));
	}

	Network::EntityStatesMessage message(0);
	int entityIndex = 0;
	while (entityIndex < entityStates.size())
	{
		int count = 0;
		for (int i = 0; i < Network::EntityStatesMessage::MaxCount && entityIndex < entityStates.size(); i++, count++, entityIndex++)
		{
			message.myData[i] = entityStates[entityIndex];
		}
		message.SetCount(static_cast<uint8_t>(count));
		myWorldServerConnection.Send(message);
		std::cout << "Responding req for entity states " << count << std::endl;
	}
}
