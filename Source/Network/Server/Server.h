#include "Socket\UDPSocket.h"
#include "Address\Address.h"
#include "Config.h"
#include "NetMessageQueue.hpp"
#include <array>

namespace Network
{
	struct UDPConnection
	{
		Address myAddress;

	};

	class Server
	{
	public:
		void Startup();
		void Update();

	private:
		UDPSocket myUDPSocket;
		std::array<UDPConnection, Constants::MAX_CLIENT_COUNT> myUDPConnections;
		NetMessageQueue<1024> myIncomingMessages;
	};
}