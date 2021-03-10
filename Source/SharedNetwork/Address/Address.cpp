#include "pch.h"
#include "Address.h"

Network::Address::Address(const std::string& aAddress, unsigned short aPort)
{
	mySockAddr.sin_family = AF_INET;
	mySockAddr.sin_port = htons(aPort);
	if (InetPtonA(AF_INET, aAddress.c_str(), &mySockAddr.sin_addr) == SOCKET_ERROR)
	{
		// TODO: Error 
	}
}

