#include "Common.h"
#include "NetMessage\NetMessageQueue.hpp"
#include <array>
#include <bitset>
#include <unordered_map>

namespace Network
{
	struct ClientData
	{
		double connectionTime = 0;
		double timeLastHeartbeatSent = 0;
		double timeLastHeartbeatReceived = 0;
	};

	class Server
	{
	public:
		void Startup();
		
		void ReceiveIncomingMessages();

		void Update(const float aDeltatime);

		void SendHeartbeat();
		
		bool HasFreeSlot() const { return myConnectedClientsCount < Constants::MAX_CLIENT_COUNT; }

	private:
		void DisconnectClient(int aClientSlot);

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
		double myTime;
	};
}