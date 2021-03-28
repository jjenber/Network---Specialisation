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

	myConnection.Init(16, Network::eNetMessageID::eNETMESSAGE_AS_HANDSHAKE, [&](
		char aData[Network::Constants::MAX_BUFFER_SIZE])
		{
			myAreaServerMessages.EnqueueReceivedBuffer(aData);
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
	myConnection.Update();
}

void WorldServer::OnAreaServerMessageReceived(Network::eMessageStatus aStatus, Network::MessageID_t aID, size_t aSize, void* aData)
{
	using namespace Network;
	if (aStatus == eMessageStatus::Success)
	{
		std::cout << "Received " << aID << std::endl;
	}
	else if (aStatus == eMessageStatus::TimedOut)
	{
		std::cout << "Timed out " << aID << std::endl;
	}
}
