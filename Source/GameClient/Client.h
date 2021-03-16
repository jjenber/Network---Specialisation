#include "Common.h"
#include "ReliableNetMessageQueue.hpp"
namespace Network
{
	class Client
	{
	public:
		void Init();
		void Update();
		void ConnectToServer();
	private:
		UDPSocket myUDPSocket;
		Address myMainServerAddress;
		ReliableNetMessageQueue<1024> myReliableMessageQueue;
	};
}