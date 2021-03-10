#include "Socket\UDPSocket.h"
#include "Address\Address.h"
#include "Config.h"

namespace Network
{
	struct ClientEntry
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
	};
}