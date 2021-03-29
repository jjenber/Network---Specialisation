#pragma once
#include "BaseConnection.h"

namespace Network
{
	typedef std::function<void(const Address& aAddress, unsigned short aSlot)> OnConnectionCallback_t;

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
		MultiConnection();

		void Init(size_t aMaxConnected, MessageID_t aHandshakeID, OnConnectionCallback_t aOnConnectionCallback);
		bool Bind(const Address& aAddress);
		bool Bind(const std::string& aIP, unsigned short aPort);

		void SetMaxConnections(size_t aMaxConnected);
		void SetHandshakeID(MessageID_t aHandshakeID);
		void SetOnConnectionCallback(OnConnectionCallback_t aOnConnectionCallback);

		template<class NetMessageType>
		void Send(const NetMessageType& aNetMessage, const Address& aAddress);

	private:
		void OnReceivedMessage(char recvBuffer[Constants::MAX_BUFFER_SIZE], const Address& aAddress) override;

		void HandleHandshakeMessage(int connectionSlot, const Address& aAddress);
		
		int GetConnectionSlot(const Address& aAddress) const;
		int FindFreeConnectionSlot() const;
		void EstablishNewConnection(int aConnectionSlot, const Address& aAddress);

		OnConnectionCallback_t	myOnConnectionCallback;

		std::vector<Connection>	myConnections;
		size_t					myMaxConnections;
		MessageID_t				myHandshakeID;
		UDPSocket				mySocket;

		const static int		ConnectionSlotNotFound = -1;
	};

	template<class NetMessageType>
	inline void MultiConnection::Send(const NetMessageType& aNetMessage, const Address& aAddress)
	{
		SendOrEnqueue(aNetMessage, aAddress);
	}
}

