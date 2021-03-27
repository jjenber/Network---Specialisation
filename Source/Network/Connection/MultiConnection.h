#pragma once
#include "BaseConnection.h"

namespace Network
{
	typedef std::function<void(eMessageStatus aStatus, Network::MessageID_t aID, size_t aSize, void* aData)> NetMessageCallback_t;

	class MultiConnection : public BaseConnection
	{
		struct Connection
		{
			inline bool IsConnected() const { return myConnectionStatus == eConnectionStatus::Connected; }
			inline bool IsFree()      const { return myConnectionStatus == eConnectionStatus::Disconnected && !myAddress.Valid(); }

			Address myAddress;
			eConnectionStatus myConnectionStatus = eConnectionStatus::Disconnected;
		};

	public:
		using BaseConnection::BaseConnection;

		void Init(size_t aMaxConnected, MessageID_t aHandshakeID, NetMessageCallback_t aCallback);
		bool Bind(const Address& aAddress);
		bool Bind(const std::string& aIP, unsigned short aPort);

		void SetMaxConnections(size_t aMaxConnected);
		void SetHandshakeID(MessageID_t aHandshakeID);

		template<class NetMessageType>
		void Send(const NetMessageType& aNetMessage, const Address& aAddress);

		void Update(const float aDeltatime);
		
	private:

		void DecodeReceived(int aConnectionSlot);
		void HandleAcknowledgement(int aConnectionSlot);
		void HandleHandshakeMessage(int connectionSlot, const Address& aAddress);
		
		int GetConnectionSlot(const Address& aAddress) const;
		int FindFreeConnectionSlot() const;
		void EstablishNewConnection(int aConnectionSlot, const Address& aAddress);

		NetMessageCallback_t			myCallback;
		ReliableNetMessageQueue			myReliableNetMessageQueue;
		NetMessageQueue<1024>			myReceivedMessages;

		std::vector<Connection>			myConnections;
		size_t							myMaxConnections;
		MessageID_t						myHandshakeID;
		
		const static int				ConnectionSlotNotFound = -1;
	};

	template<class NetMessageType>
	inline void MultiConnection::Send(const NetMessageType& aNetMessage, const Address& aAddress)
	{
		SendOrEnqueue(aNetMessage, aAddress);
	}
}

