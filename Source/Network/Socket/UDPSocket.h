#pragma once
#include "Socket.h"
namespace Network
{
	class UDPSocket : public Socket
	{
	public:
		UDPSocket();
		int Send(const char* aDataBuf, int aBufLen, const Address& aAddress);
		int Receive(const char* aDataBuf, int aBufLen, const Address& aAddress, int& aReceivedLength);
	};
}

