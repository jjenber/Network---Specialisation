#include "Common.h"
#include "NetMessageQueue.hpp"
#include <array>
#include <bitset>
#include <unordered_map>

namespace Network
{
	struct ClientData
	{
	};

	class Server
	{
	public:
		void Startup();
		
		void ReceiveIncomingMessages();

	private:
		int GetNextFreeConnectionSlot();

		int FindConnectedClientSlot(const Address& aAddress);
		
		void Decode(MessageID_t aMessageID);

		void CheckNewConnection(MessageID_t aMessageID, const Address& aAddress);

		UDPSocket myUDPSocket;
		NetMessageQueue<1024> myReceivedMessages;

		std::array<ClientData, Constants::MAX_CLIENT_COUNT> myClientData;
		std::array<Address, Constants::MAX_CLIENT_COUNT> myClientAddresses;
		std::bitset<Constants::MAX_CLIENT_COUNT> myConnectedClients;
		size_t myConnectedClientsCount;

		const static int NoClientFound = -1;
	};
}