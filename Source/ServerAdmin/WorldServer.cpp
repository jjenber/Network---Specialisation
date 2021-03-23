#include "WorldServer.h"
#include "Timer\Timer.h"
#include <iostream>
#include <tchar.h>

void WorldServer::Startup()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
	{
		std::cout << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
	}

	myTime = 0;
	mySocket.BindToLocal();
	mySocket.SetBlocking(false);
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

void WorldServer::ReceiveIncomingMessages()
{
	using namespace Network;
	Network::Address fromAddress;
	char recvBuf[Constants::MAX_BUFFER_SIZE]{};

	while (mySocket.Receive(recvBuf, fromAddress))
	{
		myIncomingNetMessages.EnqueueReceivedBuffer(recvBuf);
		eNetMessageID messageID = myIncomingNetMessages.Peek();
		if (messageID == eNETMESSAGE_AREA_SERVER_HANDSHAKE || messageID == eNETMESSAGE_CLIENT_HANDSHAKE)
		{
			HandleHandshake(messageID, fromAddress);
		}
	}

	myIncomingNetMessages.Clear();
}

void WorldServer::HandleHandshake(Network::eNetMessageID aID, const Network::Address& aAddress)
{
	Network::NetMessage msg;
	myIncomingNetMessages.Dequeue(msg);
	
	if (aID == Network::eNETMESSAGE_AREA_SERVER_HANDSHAKE)
	{
		int areaServerSlot = FindConnectedAreaServer(aAddress);
		if (areaServerSlot < 0)
		{
			areaServerSlot = FindFreeAreaServerSlot();
			if (areaServerSlot != -1)
			{
				// Connect the area server
				myAreaServers.myAddresses[areaServerSlot] = aAddress;
				myAreaServers.myConnected[areaServerSlot] = true;
				myAreaServers.myInstances[areaServerSlot] = {};
				std::cout << "Area server is now connected! " << std::endl;
			}
		}
		msg.mySenderID = areaServerSlot;
	}
	else if (aID == Network::eNETMESSAGE_CLIENT_HANDSHAKE)
	{
		std::cout << "Client attempting to connect." << std::endl;
	}

	mySocket.Send(msg, aAddress);
}

int WorldServer::FindConnectedAreaServer(const Network::Address& aAddress) const
{
	for (int i = 0; i < MAX_AREA_SERVERS; i++)
	{
		if (myAreaServers.myConnected[i] && myAreaServers.myAddresses[i] == aAddress)
		{
			return i;
		}
	}
	return -1;
}

int WorldServer::FindFreeAreaServerSlot() const
{
	for (size_t i = 0; i < MAX_AREA_SERVERS; i++)
	{
		if (!myAreaServers.myConnected[i])
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}
