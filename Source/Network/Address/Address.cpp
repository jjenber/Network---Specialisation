#include "pch.h"
#include "Address.h"
#include <iostream>

Network::Address::Address(const std::string& aAddress, unsigned short aPort)
{
	mySockAddr.sin_family = AF_INET;
	mySockAddr.sin_port = htons(aPort);
	if (InetPtonA(AF_INET, aAddress.c_str(), &mySockAddr.sin_addr) == SOCKET_ERROR)
	{
		// TODO: Error 
		std::cout << "Error when creating address." << std::endl;
	}
}

Network::Address::Address(unsigned short aPort)
{
	mySockAddr.sin_family = AF_INET;
	mySockAddr.sin_port = htons(aPort);
	mySockAddr.sin_addr.S_un.S_addr = ADDR_ANY;
}

