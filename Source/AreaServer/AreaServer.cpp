#include "AreaServer.h"
#include "NetMessage\NetMessage.hpp"
#include "Timer\Timer.h"

#include "../Game/Components/Transform.hpp"
#include "../Game/Components/UniqueID.hpp"
#include "../Game/Components/Client.hpp"
#include "../Game/Components/MigrateClient.hpp"
#include "../Game/Components/ShadowClient.hpp"
#include <iostream>

AreaServer::AreaServer() : 
	myConnectionStatus(Network::eConnectionStatus::Disconnected),
	myIsRunning(false),
	myStatus(eAreaServerStatus::Shutdown)
{
}

bool AreaServer::Startup()
{
	myWorldServerConnection.Init(mySocket);
	myWorldServerAddress = Network::Address("127.0.0.1", Network::Constants::WORLD_TO_AREA_PORT);

	myIsRunning = myWorldServerConnection.Connect(
		myWorldServerAddress,
		Network::Constants::RESEND_WAIT_TIME_MS, 
		Network::eNETMESSAGE_AS_HANDSHAKE);

	myClientConnections.Init(
		MAX_CLIENT_COUNT,
		Network::eNETMESSAGE_CLIENT_HANDSHAKE,
		[&](const Network::Address& aAddress, unsigned short aConnectionSlot) // Callback for when 
		{
			OnClientConnected(aAddress, aConnectionSlot);
		});

	if (myClientConnections.Bind("127.0.0.1", 0))
	{
		std::cout << "My inbounding client address: " << myClientConnections.GetSocket()->GetBoundAddress().ToString() << std::endl;
	}

	return myIsRunning;
}


bool AreaServer::Update(const float aDeltatime)
{
	myTime += aDeltatime;

	myWorldServerConnection.Update(aDeltatime);
	while (myWorldServerConnection.HasMessages())
	{
		HandleWorldServerMessage(myWorldServerConnection.Peek());
	}

	myClientConnections.Update(aDeltatime);
	while (myClientConnections.HasMessages())
	{
		HandleClientMessage(myClientConnections.Peek());
	}

	myGame.Update(aDeltatime);
	
	SyncClients(aDeltatime);

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

		Network::AreaServerInitialized initializedMsg(myClientConnections.GetSocket()->GetBoundAddress());
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

		ShadowClient incomingClient;
		incomingClient.myUniqueID    = msg.myClientState.myUniqueID;
		incomingClient.myToken       = msg.myToken;
		incomingClient.myLocalEntity = myGame.InstantiateShadowClient(incomingClient.myUniqueID, msg.myClientState.myPosition, msg.myClientState.myVelocity);

		myQueuedClients.push_back(incomingClient);

	}	break;

	case Network::eNETMESSAGE_R_CLIENT_EXIT_AREA:
	{
		Network::ClientExitAreaMessage msg;
		myWorldServerConnection.ReadNextMessage(msg);
		
		myGame.DestroyEntity(msg.myUniqueID);
		for (int i = 0; i < myClientData.size(); i++)
		{
			if (myClientData[i].myUniqueID == msg.myUniqueID)
			{
				myClientData[i] = ClientData{};
				break;
			}
		}
	}	break;

	default:
		std::cout << "Message ID: " << (int)aMessageID << " not handled." << std::endl;
		break;
	}
}

void AreaServer::HandleClientMessage(Network::MessageID_t aMessageID)
{
	switch (aMessageID)
	{
	case Network::eNETMESSAGE_CLIENT_MOVE:
	{
		Network::ClientMoveMessage msg;
		myClientConnections.ReadNextMessage(msg);

		if (myClientData[msg.mySenderID].myIsConnected && myClientData[msg.mySenderID].myIsValidated)
		{
			myGame.SetClientVelocity(myClientData[msg.mySenderID].myLocalID, msg.myVelocity);
			myClientData[msg.mySenderID].myLastMessageTime = myTime;
		}
		break;
	}

	case Network::eNETMESSAGE_R_CLIENT_VALIDATE_TOKEN:
	{
		Network::ClientValidateTokenMessage msg;
		myClientConnections.ReadNextMessage(msg);

		for (int i = 0; i < myQueuedClients.size(); i++)
		{
			const ShadowClient& queuedClient = myQueuedClients[i];

			if (queuedClient.myToken == msg.myToken && queuedClient.myUniqueID == msg.myUniqueID)
			{
				myGame.GetRegistry().remove_if_exists<components::ShadowClient>(queuedClient.myLocalEntity);
				
				ClientData& clientData       = myClientData[msg.mySenderID];
				clientData.myLocalID         = queuedClient.myLocalEntity;
				clientData.myUniqueID        = msg.myUniqueID;
				clientData.myIsValidated     = true;
				clientData.myIsMigrating     = false;
				clientData.myIsShadow        = false;
				clientData.myLastMessageTime = myTime;

				myQueuedClients.erase(myQueuedClients.begin() + i);
				break;
			}
		}

	}   break;
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
	std::vector<Network::EntityState> entityStates;
	auto view = myGame.GetRegistry().view<const components::UniqueID, const components::Transform>(entt::exclude<components::Client>);

	entityStates.reserve(view.size_hint());

	for (auto&& [localID, uniqueID, transform] : view.each())
	{
		entityStates.push_back(Network::EntityState(transform.myPosition.x, transform.myPosition.z, uniqueID.myUniqueID));
	}

	Network::EntityStatesMessage message;

	int entityIndex = 0;
	while (entityIndex < entityStates.size())
	{
		int entityCount = 0;
		for (int i = 0; i < Network::EntityStatesMessage::MaxCount && entityIndex < entityStates.size(); i++, entityCount++, entityIndex++)
		{
			message.myData[i] = entityStates[entityIndex];
		}
		message.SetCount(static_cast<uint8_t>(entityCount));

		myWorldServerConnection.Send(message);
	}
}

void AreaServer::OnClientConnected(const Network::Address& aAddress, uint16_t aConnectionSlot)
{
	std::cout << "Client connected" << std::endl;
	myClientData[aConnectionSlot].myIsConnected = true;
	myClientData[aConnectionSlot].myAddress = aAddress;
	myClientData[aConnectionSlot].myLastMessageTime = myTime;
	myClientData[aConnectionSlot].myIsShadow = true;
}

void AreaServer::SyncClients(const float aDeltatime)
{
	mySyncClientsTimer += aDeltatime;
	if (mySyncClientsTimer >= 0.1f)
	{
		std::vector<Network::EntityState> entityStates;
		Network::ClientServerPosition msg;

		auto& registry = myGame.GetRegistry();

		for (auto& data : myClientData)
		{
			if (data.myIsConnected && data.myIsValidated && !data.myIsShadow)
			{
				msg.myPosition = myGame.GetPosition(data.myLocalID);
				myClientConnections.Send(msg, data.myAddress);

				entityStates.push_back(Network::EntityState(msg.myPosition.x, msg.myPosition.z, data.myUniqueID));

				auto migrate = registry.try_get<components::MigrateClient>(data.myLocalID);
				if (migrate && !data.myIsMigrating)
				{
					MigrateClient(data, *migrate);
					registry.remove<components::MigrateClient>(data.myLocalID);
				}
				else if (myTime - data.myLastMessageTime > Network::Constants::CLIENT_TIME_OUT_S)
				{
					ClientTimedOut(data);
				}
			}
		}

		mySyncClientsTimer = 0.f;

		// Sync all client states to World Server
		Network::EntityStatesMessage message;
		message.myMessageID = Network::eNETMESSAGE_AS_CLIENT_STATES;

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
}

void AreaServer::MigrateClient(ClientData& aClientData, const components::MigrateClient& aComponent)
{
	if (!aClientData.myIsMigrating)
	{
		aClientData.myIsMigrating = true;

		Network::ClientMigrateMessage msg(aClientData.myUniqueID, aComponent.myX, aComponent.myY);
		msg.myPosition = myGame.GetPosition(aClientData.myLocalID);
		msg.myVelocity = myGame.GetVelocity(aClientData.myLocalID);
		myWorldServerConnection.Send(msg);
	}
}

void AreaServer::ClientTimedOut(ClientData& aClientData)
{
	Network::ClientTimedOutMessage msg(aClientData.myUniqueID);
	myWorldServerConnection.Send(msg);
	myGame.GetRegistry().destroy(aClientData.myLocalID);
	aClientData = ClientData{};
}
