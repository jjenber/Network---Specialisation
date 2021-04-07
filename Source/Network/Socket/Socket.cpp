#include "pch.h"
#include "Socket.h"
#include <iostream>

void Network::Socket::SetBlocking(bool aBlock)
{
	u_long n = aBlock ? FALSE : TRUE;
	if (ioctlsocket(mySocket, FIONBIO, &n) == SOCKET_ERROR)
	{
		// TODO: Error
	}
}

bool Network::Socket::Bind(const Address& aAddress)
{
	sockaddr_in sockAddr{};
	aAddress.ToSockAddr(sockAddr);
	if (bind(mySocket, (const sockaddr*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		std::cout << "Error when binding socket. " << WSAGetLastError() << std::endl;
		return false;
	}
	myBoundAddress = aAddress;
	return true;
}

bool Network::Socket::BindToLocal(uint32_t aPort)
{
	Address addr = Address("127.0.0.1", aPort);
	return Bind(addr);
}

void Network::Socket::Close()
{
	closesocket(mySocket);
}

const Network::Address& Network::Socket::GetBoundAddress()
{
	if (!myBoundAddress.Valid())
	{
		sockaddr_in addr{};
		int l = sizeof(addr);
		getsockname(mySocket, (sockaddr*)&addr, &l);
		myBoundAddress.SetFromSockAddr(addr);
	}
	return myBoundAddress;
}