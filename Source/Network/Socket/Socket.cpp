#include "pch.h"
#include "Socket.h"
#include "Address/Address.h"
#include <iostream>

void Network::Socket::SetBlocking(bool aBlock)
{
	u_long n = aBlock ? FALSE : TRUE;
	if (ioctlsocket(mySocket, FIONBIO, &n) == SOCKET_ERROR)
	{
		// TODO: Error
	}
}

void Network::Socket::Bind(const Address& aAddress)
{
	if (bind(mySocket, (const sockaddr*)&aAddress, sizeof(aAddress)) == SOCKET_ERROR)
	{
		// TODO: Error
	}
}

void Network::Socket::BindToLocal()
{
	Address addr = Address(Constants::DEFAULT_PORT);
	if (bind(mySocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		// TODO: Error
		std::cout << "Error binding to local." << std::endl;
	}
}

void Network::Socket::Close()
{
	closesocket(mySocket);
}
