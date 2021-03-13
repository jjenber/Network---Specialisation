#include "pch.h"
#include "Socket.h"
#include "Address/Address.h"

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
	sockaddr_in hint;
	ZeroMemory(&hint, sizeof(SOCKADDR_IN));
	hint.sin_family = AF_INET;
	hint.sin_addr.S_un.S_addr = ADDR_ANY;
	hint.sin_port = htons(ADDR_ANY);

	if (bind(mySocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
	{
		// TODO: Error
	}
}

void Network::Socket::Close()
{
	closesocket(mySocket);
}
