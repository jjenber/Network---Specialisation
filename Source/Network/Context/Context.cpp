#include "pch.h"
#include "Context.h"
#include <iostream>

namespace Network
{
	Context::Context()
	{
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != NO_ERROR)
		{
			std::cout << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
		}
	}
	
	Context::~Context()
	{
		WSACleanup();
	}
}
