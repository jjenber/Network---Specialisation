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
		bool Bind(const Address& aAddress);
		bool BindToLocal();
		void Close();
		const Address& GetBoundAddress();
	protected:
		SOCKET mySocket = 0;
		Address myBoundAddress;
	};
}

