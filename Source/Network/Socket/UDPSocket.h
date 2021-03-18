#pragma once
#include "Socket.h"
namespace Network
{
	class NetMessage;
	class UDPSocket : public Socket
	{
	public:
		UDPSocket();
		int Send(const char* aDataBuf, int aBufLen, const Address& aAddress);
		int Send(const NetMessage& aMessage, const Address& aAddress);
		bool Receive(const char* aDataBuf, int aBufLen, Address& aAddress, int& aReceivedLength);
	};
}

