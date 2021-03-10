#include "pch.h"
#include "UDPSocket.h"
#include "Address/Address.h"
#include <iostream>

Network::UDPSocket::UDPSocket()
{
	if (mySocket = socket(AF_INET, SOCK_DGRAM, 0) == INVALID_SOCKET)
	{
		// TODO: Error
		std::cout << "Failed to create UDP socket" << std::endl;
	}
}

int Network::UDPSocket::Send(const char* aDataBuf, int aBufLen, const Address& aAddress)
{
	return sendto(mySocket, aDataBuf, aBufLen, 0, (sockaddr*)&aAddress, sizeof(aAddress));
}

int Network::UDPSocket::Receive(const char* aDataBuf, int aBufLen, const Address& aAddress, int& aReceivedLength)
{
	return recvfrom(mySocket, (char*)aDataBuf, aBufLen, 0, (sockaddr*)&aAddress, &aReceivedLength);
}

