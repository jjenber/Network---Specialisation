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

		/// Bind to port 0 to receive available port from the system.
		bool Bind(const Address& aAddress);
		
		bool BindToLocal(uint32_t aPort);
		
		void Close();

		const Address& GetBoundAddress();

	protected:
		SOCKET mySocket = 0;
		Address myBoundAddress;
	};
}

