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
		std::cout << "Error when binding socket. " << WSAGetLastError() << std::endl;
	}
}

void Network::Socket::BindToLocal()
{
	Address addr = Address("127.0.0.1", Constants::DEFAULT_PORT);
	sockaddr_in sockAddr;
	addr.ToSockAddr(sockAddr);
	if (bind(mySocket, (sockaddr*)&sockAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		std::cout << "Error binding to localhost. " << WSAGetLastError() << std::endl;
	}
}

void Network::Socket::Close()
{
	closesocket(mySocket);
}
