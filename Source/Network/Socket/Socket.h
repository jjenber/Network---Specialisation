#pragma once
#include <WinSock2.h>

namespace Network
{
	class Address;
	class Socket
	{
	public:
		virtual ~Socket() {}

		void SetBlocking(bool aBlock);
		void Bind(const Address& aAddress);
		void BindToLocal();
		void Close();
	protected:
		SOCKET mySocket = 0;
	};
}

