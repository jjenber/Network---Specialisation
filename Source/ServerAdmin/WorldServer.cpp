#include "WorldServer.h"
#include "Timer/Timer.h"

#include <CommonUtilities\Random\Random.h>
#include <iostream>
#include <tchar.h>

void WorldServer::Startup()
{
	myTime = 0;
	myAreaServerConnection.Init(
		MAX_AREA_SERVERS, 
		Network::eNetMessageID::eNETMESSAGE_AS_HANDSHAKE, 
		[&](const Network::Address& aAddress, unsigned short aConnectionSlot)
			{
				OnAreaServerConnected(aConnectionSlot, aAddress);
			});
	
	myClientConnections.Init(
		MAX_CLIENT_COUNT,
		Network::eNETMESSAGE_CLIENT_HANDSHAKE,
		[&](const Network::Address& aAddress, unsigned short aConnectionSlot)
			{
				OnClientConnected(aConnectionSlot, aAddress);
			});

	if (myAreaServerConnection.Bind("127.0.0.1", Network::Constants::WORLD_TO_AREA_PORT))
	{
		myAreaServerConnection.GetSocket().SetBlocking(false);
		std::cout << "Server is listening for area servers at " << myAreaServerConnection.GetSocket().GetBoundAddress().ToString() << std::endl;
	}

	if (myClientConnections.Bind("127.0.0.1", Network::Constants::WORLD_TO_CLIENT_PORT))
	{
		myClientConnections.GetSocket().SetBlocking(false);
		std::cout << "Server is listening for clients at " << myClientConnections.GetSocket().GetBoundAddress().ToString() << std::endl;
	}
}

void WorldServer::InstantiateAreaServers()
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	wchar_t cmd[] = L"-cmd";
	if (!CreateProcess(
		_T("AreaServer_Debug.exe"),  // the path
		cmd,                         // Command line
		NULL,                        // Process handle not inheritable
		NULL,                        // Thread handle not inheritable
		FALSE,                       // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,          // No creation flags
		NULL,                        // Use parent's environment block
		NULL,                        // Use parent's starting directory 
		&si,                         // Pointer to STARTUPINFO structure
		&pi                          // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	))
	{
		std::cout << "Failed to create process: " << GetLastError() << std::endl;
	}
}

void WorldServer::Update(const float aDeltatime)
{
	myTime += aDeltatime;
	myAreaServerConnection.Update(aDeltatime);
	while (myAreaServerConnection.HasMessages())
	{
		HandleAreaServerMessages();
	}
	myAreaServerConnection.ClearMessages();
	SendRequestEntityStateRequests(aDeltatime);

	myClientConnections.Update(aDeltatime);
	while (myClientConnections.HasMessages())
	{
		
	}
}

bool WorldServer::CanStartAreaServer() const
{
	for (auto& areaServer : myAreaServerInstances)
	{
		if (areaServer.myStatus == eAreaServerStatus::Loading)
			return false;
	}
	return true;
}

void WorldServer::HandleAreaServerMessages()
{
	Network::MessageID_t id = myAreaServerConnection.Peek();
	switch (id)
	{

	case Network::eNETMESSAGE_R_AS_STATUS:
	{
		Network::AreaServerStatus msg;
		myAreaServerConnection.ReadNextMessage(msg);
		myAreaServerInstances[msg.mySenderID].myStatus = static_cast<eAreaServerStatus>(msg.myStatus);
		
		std::cout << "Received status msg "  << (int)msg.myStatus << std::endl;
		break;
	}

	case Network::eNETMESSAGE_R_AS_INITIALIZED:
	{
		Network::AreaServerInitialized msg;
		myAreaServerConnection.ReadNextMessage(msg);

		AreaServerInstance& instance = myAreaServerInstances[msg.mySenderID];

		instance.myStatus = eAreaServerStatus::Running;
		instance.myClientConnectionAddress = msg.myClientConnectionAddress;

		std::cout << "Area server initialized. Its inbounding client connection address: "  << msg.myClientConnectionAddress.ToString() << std::endl;
		break;
	}

	case Network::eNETMESSAGE_R_AS_REQUEST_IDS:
	{
		Network::RequestUniqueIDs msg;
		myAreaServerConnection.ReadNextMessage(msg);
		 
		std::vector<entt::entity> entities;
		myGameWorld.InstantiateEntities(msg.myCount, entities);

		Network::ResponseUniqueIDs response(msg.myCount);
		
		for (int mapItr = 0, uniqueIDItr = 0; uniqueIDItr < msg.myCount; mapItr += 2, uniqueIDItr++)
		{
			response.myMappedIDs[mapItr]     = msg.myLocalIDs[uniqueIDItr];
			response.myMappedIDs[mapItr + 1] = static_cast<uint32_t>(entities[uniqueIDItr]);
		}

		AreaServerInstance& areaServer = myAreaServerInstances[msg.mySenderID];
		areaServer.myEntities.insert(areaServer.myEntities.end(), entities.begin(), entities.end());

		myAreaServerConnection.Send(response, myAreaServerInstances[msg.mySenderID].myAddress);
		break;
	}

	case Network::eNETMESSAGE_AS_CLIENT_STATES:
	case Network::eNETMESSAGE_AS_RESPONSE_ENTITY_STATES:
	{
		Network::EntityStatesMessage msg(0);
		myAreaServerConnection.ReadNextMessage(msg);

		int region = myAreaServerInstances[msg.mySenderID].myRegions.front();
		int regionOffsetX = (region % REGION_ROW_COL) * REGION_SIZE;
		int regionOffsetY = (region / REGION_ROW_COL) * REGION_SIZE;

		for (int i = 0; i < msg.myCount; i++)
		{
			auto& state = msg.myData[i];
			myGameWorld.UpdateEntityState(state.myUniqueID,  regionOffsetX + state.myX, regionOffsetY + state.myZ);
		}
		break;
	}
	default:
		std::cout << (int)id << " unhandled message from area server" << std::endl;
		break;
	}
}

void WorldServer::OnAreaServerConnected(int aAreaServerID, const Network::Address& aAddress)
{
	myAreaServerInstances[aAreaServerID].myAddress = aAddress;
	DeployAreaServer(aAreaServerID);
}

void WorldServer::HandleClientMessages()
{
	Network::MessageID_t id = myAreaServerConnection.Peek();
	switch (id)
	{
	default:
		std::cout << "Unhandled message from Client: " << (int)id << std::endl;
	}
}

void WorldServer::OnClientConnected(int aClientID, const Network::Address& aAddress)
{
	Network::Address areaServerAddress;
	Network::Address areaServerClientAddress;

	CommonUtilities::Vector3f aPosition;

	aPosition.x = Random::Range(0.f, REGION_SIZEF);
	aPosition.z = Random::Range(0.f, REGION_SIZEF);
	
	entt::id_type id = myGameWorld.InstantiateClient(aPosition);

	int region = -1;

	for (int i = 0; i < myAreaServerInstances.size(); i++)
	{
		if (myAreaServerInstances[i].myStatus == eAreaServerStatus::Running)
		{
			AreaServerInstance& areaServer = myAreaServerInstances[i];

			region                  = areaServer.myRegions[0];
			areaServerAddress       = areaServer.myAddress;
			areaServerClientAddress = areaServer.myClientConnectionAddress;

			areaServer.myClients.push_back(entt::entity(id));
		}
	}

	std::cout << "Connecting Client to slot " << 0 << std::endl;

	Client& client = myClients[aClientID];
	client.myRegion = region;
	client.myAddress = aAddress;

	Network::ClientEnterAreaMessage message(
		CommonUtilities::Vector3<uint16_t>{ static_cast<uint16_t>(aPosition.x), static_cast<uint16_t>(aPosition.y), static_cast<uint16_t>(aPosition.z) }, 
		areaServerClientAddress.GetIP(), 
		areaServerClientAddress.GetPort(),
		id,
		0,
		aAddress,
		Random::Range(1, INT_MAX));

	myClientConnections.Send(message, aAddress);
	myAreaServerConnection.Send(message, areaServerAddress);

	std::cout << "Sending area server-to-client address: " << areaServerClientAddress.ToString() << std::endl;
}

void WorldServer::DeployAreaServer(unsigned short aAreaServerID)
{
	int region = myGameWorld.GetUnassignedRegionIndex();
	
	if (region >= 0)
	{
		myGameWorld.InitRegion(region, aAreaServerID);
		myAreaServerInstances[aAreaServerID].myRegions.push_back(region);

		Network::DeployAreaServer message(region);
		myAreaServerConnection.Send(message, myAreaServerInstances[aAreaServerID].myAddress);
		myAreaServerInstances[aAreaServerID].myStatus = eAreaServerStatus::Loading;

		std::cout << "Deploying Region " << region << " on AreaServer " << aAreaServerID << std::endl;
	}
	else
	{
		std::cout << "No avaliable regions" << std::endl;
	}
}

void WorldServer::SendRequestEntityStateRequests(const float aDeltatime)
{
	Network::NetMessage msg(Network::eNETMESSAGE_AS_REQUEST_ENTITY_STATES);

	for (auto& instance : myAreaServerInstances)
	{
		if (instance.myStatus == eAreaServerStatus::Running && myTime - instance.myLastMessage > 1.5f)
		{
			instance.myLastMessage = myTime;
			myAreaServerConnection.Send(msg, instance.myAddress);
		}
	}
}
