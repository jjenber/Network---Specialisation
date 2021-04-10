#pragma once
#include "BaseConnection.h"

namespace Network
{
	class UnaryConnection : public BaseConnection
	{
	public:
		bool Connect(const Address& aAddress, float aTimeoutInSeconds, eNetMessageID aHandshakeID);
		
		void Disconnect();

		template<class NetMessageType>
		void Send(const NetMessageType& aNetMessage);

		eConnectionStatus GetConnectionStatus() const { return myConnectionStatus; }

		ClientSlot_t GetConnectionSlot() const { return mySlot; }

	private:
		void OnReceivedMessage(char aBuffer[Constants::MAX_BUFFER_SIZE], const Network::Address& aFromAddress);

		Address				myConnectedAddress;
		Address				myAddress;
		eConnectionStatus	myConnectionStatus = eConnectionStatus::Disconnected;
		ClientSlot_t		mySlot = std::numeric_limits<ClientSlot_t>::max();
	};

	template<class NetMessageType>
	inline void UnaryConnection::Send(const NetMessageType& aNetMessage)
	{
		static_assert(std::is_base_of_v<NetMessage, NetMessageType>, "NetMessageType must derive from NetMessage");
		((NetMessage&)(aNetMessage)).mySenderID = mySlot;
		SendOrEnqueue(aNetMessage, myConnectedAddress);
	}
}

