#include "Common.h"
#include "NetMessage\NetMessageQueue.hpp"
#include "NetMessage\ReliableNetMessageQueue.h"
#include <atomic>

enum eConnectionStatus
{
	NotConnected,
	Connecting,
	Connected,
	ConnectionFailed,
	ConnectionRejected,
	Disconnecting,
	Disconnected,
};

namespace Network
{
	class Client
	{
	public:
		void Init();
		void Disconnect();
		void Update();
		void ConnectToServer();

		int GetClientSlot() const { return myClientSlot; }
		eConnectionStatus GetConnectionStatus() const { return myConnectionStatus; }

	private:
		void ReceiveIncomingMessages();
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