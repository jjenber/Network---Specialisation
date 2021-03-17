#include "Common.h"
#include "NetMessageQueue.hpp"

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
	private:
		void RecieveIncomingMessages();
		void Decode(MessageID_t aMessageID);
		void DecodeReliable(MessageID_t aMessageID);

		UDPSocket myUDPSocket;
		Address myMainServerAddress;
		NetMessageQueue<1024> myReceivedMessages;
		std::atomic<eConnectionStatus> myConnectionStatus = eConnectionStatus::NotConnected;
		
		int myClientSlot = INT_MAX;
	};
}