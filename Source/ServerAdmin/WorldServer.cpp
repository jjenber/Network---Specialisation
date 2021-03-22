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
		_T("AreaServer_Debug.exe"),     // the path
		cmd,                 // Command line
		NULL,                // Process handle not inheritable
		NULL,                // Thread handle not inheritable
		FALSE,               // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,  // No creation flags
		NULL,                // Use parent's environment block
		NULL,                // Use parent's starting directory 
		&si,                 // Pointer to STARTUPINFO structure
		&pi                  // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	))
	{
		std::cout << "Failed to create process: " << GetLastError() << std::endl;
	}

	
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
