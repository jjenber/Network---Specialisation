#include "WorldServer.h"
#include "Timer\Timer.h"
#include <iostream>
#include <tchar.h>

void WorldServer::Startup()
{
	myTime = 0;
	myConnection.Init(
		MAX_AREA_SERVERS, 
		Network::eNetMessageID::eNETMESSAGE_AS_HANDSHAKE, 
		[&](const Network::Address& aAddress, unsigned short aConnectionSlot)
			{
				OnAreaServerConnected(aConnectionSlot, aAddress);
			});
	
	
	if (myConnection.Bind("127.0.0.1", Network::Constants::DEFAULT_PORT))
	{
		myConnection.GetSocket().SetBlocking(false);
		std::cout << "Server is listening at " << myConnection.GetSocket().GetBoundAddress().ToString() << std::endl;
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

	wchar_t cmd[] = L"-testing";
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
	myConnection.Update(aDeltatime);
	while (myConnection.HasMessages())
	{
		HandleAreaServerMessages();
	}
	myConnection.ClearMessages();

	SendRequestEntityStateRequests(aDeltatime);
}

void WorldServer::HandleAreaServerMessages()
{
	Network::MessageID_t id = myConnection.Peek();
	switch (id)
	{
	case Network::eNETMESSAGE_R_AS_STATUS:
	{
		Network::AreaServerStatus msg;
		myConnection.ReadNextMessage(msg);
		myInstances[msg.mySenderID].myStatus = static_cast<eAreaServerStatus>(msg.myStatus);

		std::cout << "Area Server [" << msg.mySenderID << "] status: " << (int)msg.myStatus << std::endl;

		break;
	}
	case Network::eNETMESSAGE_R_AS_REQUEST_IDS:
	{
		Network::RequestUniqueIDs msg;
		myConnection.ReadNextMessage(msg);
		
		std::cout << "Received request for " << (int)msg.myCount << " ids." << std::endl;

		std::vector<entt::entity> entities;
		myGameWorld.InstantiateEntities(msg.myCount, entities);
		Network::ResponseUniqueIDs response(msg.myCount);
		
		for (int mapItr = 0, uniqueIDItr = 0; uniqueIDItr < msg.myCount; mapItr += 2, uniqueIDItr++)
		{
			response.myMappedIDs[mapItr]     = msg.myLocalIDs[uniqueIDItr];
			response.myMappedIDs[mapItr + 1] = static_cast<uint32_t>(entities[uniqueIDItr]);
		}

		myConnection.Send(response, myInstances[msg.mySenderID].myAddress);
		break;
	}
	default:
		std::cout << (int)id << " unhandled message from area server" << std::endl;
		break;
	}
}

void WorldServer::OnAreaServerConnected(int aAreaServerID, const Network::Address& aAddress)
{
	myInstances[aAreaServerID].myAddress = aAddress;
	DeployAreaServer(aAreaServerID);
}

void WorldServer::DeployAreaServer(unsigned short aAreaServerID)
{
	int region = myGameWorld.GetUnassignedRegionIndex();
	myInstances[aAreaServerID].myRegions.push_back(region);

	Network::DeployAreaServer message(region);
	myConnection.Send(message, myInstances[aAreaServerID].myAddress);
	std::cout << "Deploying Region " << region << " on AreaServer " << aAreaServerID << std::endl;
}

void WorldServer::SendRequestEntityStateRequests(const float aDeltatime)
{
	static float aTimer = 0.f;
	aTimer += aDeltatime;
	if (aTimer > 1.5f)
	{
		Network::ReliableNetMessage msg(Network::eNETMESSAGE_R_AS_REQUEST_ENTITY_STATES);
		for (const auto& instance : myInstances)
		{
			if (instance.myStatus == eAreaServerStatus::Running)
			{
				myConnection.Send(msg, instance.myAddress);
			}
		}
		aTimer = 0.f;
	}
}
