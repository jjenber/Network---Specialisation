#include "Common.h"
#include "NetMessageQueue.hpp"
#include <array>
#include <unordered_map>

namespace Network
{
	struct UDPConnection
	{
		Address myAddress;
		bool myConnected = false;
	};

	class Server
	{
	public:
		void Startup();
		
		void ReceiveIncomingMessages();
	private:
		int GetNextFreeConnectionSlot();
		void Decode(MessageID_t aMessageID);
		void DecodeReliable(MessageID_t aMessageID);

		UDPSocket myUDPSocket;
		NetMessageQueue<1024> myReceivedMessages;
		std::array<UDPConnection, Constants::MAX_CLIENT_COUNT> myUDPConnections;
		std::unordered_map<Address, UDPConnection> mds;
	};
}