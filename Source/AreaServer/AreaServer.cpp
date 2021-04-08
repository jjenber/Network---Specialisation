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

	myClientConnections.Init(
		MAX_CLIENT_COUNT,
		Network::eNETMESSAGE_CLIENT_HANDSHAKE,
		[&](const Network::Address& aAddress, unsigned short aConnectionSlot)
		{
			OnClientConnected(aAddress, aConnectionSlot);
		});

	if (myClientConnections.Bind("127.0.0.1", 0))
	{
		std::cout << "My inbounding client address: " << myClientConnections.GetSocket().GetBoundAddress().ToString() << std::endl;
	}


	return myIsRunning;
}


bool AreaServer::Update(const float aDeltatime)
{
	myWorldServerConnection.Update(aDeltatime);

	while (myWorldServerConnection.HasMessages())
	{
		HandleWorldServerMessage(myWorldServerConnection.Peek());
	}

	myClientConnections.Update(aDeltatime);
	while (myClientConnections.HasMessages())
	{
		
	}

	myGame.Update(aDeltatime);

	return myIsRunning;
}

void AreaServer::HandleWorldServerMessage(Network::MessageID_t aMessageID)
{
	switch (aMessageID)
	{
	case Network::eNETMESSAGE_R_AS_DEPLOY:
	{
		Network::DeployAreaServer incomingMsg;
		myWorldServerConnection.ReadNextMessage(incomingMsg);
		
		myGame.Init(incomingMsg.myRegionID);

		Network::AreaServerInitialized initializedMsg(myClientConnections.GetSocket().GetBoundAddress());
		myWorldServerConnection.Send(initializedMsg);

		SendIDRequests();

	}	break;

	case Network::eNETMESSAGE_R_AS_REQUEST_IDS_RESPONSE:
	{
		Network::ResponseUniqueIDs response;
		myWorldServerConnection.ReadNextMessage(response);

		for (int i = 0; i < response.myCount * 2; i += 2)
		{
			myGame.SetUniqueID(response.myMappedIDs[i], response.myMappedIDs[i + 1]);
		}

	}	break;

	case Network::eNETMESSAGE_AS_REQUEST_ENTITY_STATES:
	{
		Network::NetMessage msg;
		myWorldServerConnection.ReadNextMessage(msg);

		SendEntityStates();

	}	break;

	case Network::eNETMESSAGE_R_CLIENT_ENTER_AREA:
	{
		Network::ClientEnterAreaMessage msg;
		myWorldServerConnection.ReadNextMessage(msg);

		std::cout << "A client entered the area: " << msg.myClientAddress.ToString() << std::endl;
		myGame.InstantiateClient(msg.myPosition, msg.myUniqueID);
		
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

	Network::EntityStatesMessage message;

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
	}
}

void AreaServer::OnClientConnected(const Network::Address& aAddress, unsigned short aConnectionSlot)
{

}
