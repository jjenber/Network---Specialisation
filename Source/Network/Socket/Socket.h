#pragma once
#include <WinSock2.h>
#include "Address\Address.h"
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
		const Address& GetBoundAddress();
	protected:
		SOCKET mySocket = 0;
		Address myBoundAddress;
	};
}

