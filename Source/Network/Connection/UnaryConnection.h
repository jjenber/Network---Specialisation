#pragma once
#include "BaseConnection.h"

namespace Network
{
	class UnaryConnection : public BaseConnection
	{
	public:
		using BaseConnection::BaseConnection;

		bool Connect(const Address& aAddress, float aTimeoutInSeconds, eNetMessageID aHandshakeID);

		template<class NetMessageType>
		void Send(const NetMessageType& aNetMessage);

	private:
		void OnReceivedMessage(char aBuffer[Constants::MAX_BUFFER_SIZE], const Network::Address& aFromAddress);

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
		SendOrEnqueue(aNetMessage, myConnectedAddress);
	}
}

