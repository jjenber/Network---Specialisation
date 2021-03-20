#include "Common.h"
#include "NetMessage\NetMessageQueue.hpp"
#include "NetMessage\ReliableNetMessageQueue.h"

enum eConnectionStatus
{
	NotConnected,
	Connecting,
	Connected,
	ConnectionFailed,
	ConnectionRejected,
};

namespace Network
{
	class Client
	{
	public:
		void Init();
		void Update();
		void ConnectToServer();

		int GetClientSlot() const { return myClientSlot; }
	private:
		void RecieveIncomingMessages();
		void Decode(MessageID_t aMessageID);
		void DecodeReliable(MessageID_t aMessageID);

		UDPSocket myUDPSocket;
		Address myMainServerAddress;
		NetMessageQueue<1024> myReceivedMessages;
		ReliableNetMessageQueue myReliableMessageQueue;
		std::atomic<eConnectionStatus> myConnectionStatus = eConnectionStatus::NotConnected;
		
		int myClientSlot = INT_MAX;
	};
}