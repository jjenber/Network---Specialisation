#include "pch.h"
#include "UDPSocket.h"
#include "Address/Address.h"
#include "NetMessage.h"
#include <iostream>

Network::UDPSocket::UDPSocket()
{
	if ((mySocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Failed to create UDP socket. " << WSAGetLastError() << std::endl;
		return;
	}
}

int Network::UDPSocket::Send(const char* aDataBuf, int aBufLen, const Address& aAddress)
{
	sockaddr_in sockAddr;
	aAddress.ToSockAddr(sockAddr);
	return sendto(mySocket, aDataBuf, aBufLen, 0, (sockaddr*)&sockAddr, sizeof(sockAddr));
}

int Network::UDPSocket::Send(const NetMessage& aMessage, const Address& aAddress)
{
	char buf[Constants::MAX_BUFFER_SIZE]{};
	buf[0] = unsigned char(1); // Message count
	memcpy(buf + 1, &aMessage.myMessageID, aMessage.mySize);
	return Send(buf, aMessage.mySize, aAddress);
}

bool Network::UDPSocket::Receive(const char* aDataBuf, Address& aAddress)
{
	sockaddr_in addr{};
	int length = sizeof(addr);
	int result = recvfrom(mySocket, (char*)aDataBuf, Constants::MAX_BUFFER_SIZE, 0, (sockaddr*)&addr, &length);
	
	aAddress.SetFromSockAddr(addr);
	
	return 
		result != SOCKET_ERROR && 
		WSAGetLastError() != WSAEWOULDBLOCK;
}

