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

		void Update(const float aDeltatime);
		void SendHeartbeat();
	private:
		int GetNextFreeClientSlot();

		int FindConnectedClientSlot(const Address& aAddress);
		
		void Decode(MessageID_t aMessageID);

		void CheckNewConnection(MessageID_t aMessageID, const Address& aAddress, int aClientSlot);

		UDPSocket myUDPSocket;
		NetMessageQueue<1024> myReceivedMessages;

		std::array<ClientData, Constants::MAX_CLIENT_COUNT> myClientData;
		std::array<Address, Constants::MAX_CLIENT_COUNT> myClientAddresses;
		std::bitset<Constants::MAX_CLIENT_COUNT> myConnectedClients;
		size_t myConnectedClientsCount;

		const static int ClientNotFound = -1;
	};
}