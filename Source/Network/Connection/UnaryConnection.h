#pragma once
#include "BaseConnection.h"

namespace Network
{
	class UnaryConnection : public BaseConnection
	{
	public:
		using BaseConnection::BaseConnection;

		bool Connect(const Address& aAddress, float aTimeoutInSeconds, eNetMessageID aHandshakeID);
		void ConnectAsync(const Address& aAddress, std::function<void()> myCallback);

		template<class NetMessageType>
		void Send(const NetMessageType& aNetMessage);
		
		void SetReceiveMessageCallback(NetMessageCallback_t aCallback);

		void Update(const float aDeltatime);
	private:
		NetMessageQueue<256>	myMessageQueue;
		ReliableNetMessageQueue myReliableNetMessageQueue;
		eConnectionStatus		myConnectionStatus;
		Address					myConnectedAddress;
		Address					myAddress;
		int						mySlot;
	};

	template<class NetMessageType>
	inline void UnaryConnection::Send(const NetMessageType& aNetMessage)
	{
		static_assert(std::is_base_of_v<NetMessage, NetMessageType>, "NetMessageType must derive from NetMessage");
		static_cast<NetMessage&>(aNetMessage).mySenderID = mySlot;

		if constexpr (std::is_base_of_v<ReliableNetMessage, NetMessageType>)
		{
			myReliableNetMessageQueue.Enqueue(aNetMessage, myConnectedAddress, 10, 100.f);
		}
		else
		{
			myMessageQueue.Enqueue(aNetMessage);
		}
	}
}

