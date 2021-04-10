#pragma once
#include "Common.h"
#include "NetMessage\NetMessageQueue.hpp"
#include "NetMessage\ReliableNetMessageQueue.h"
#include <functional>

namespace Network
{
	class BaseConnection
	{
	public:
		void Init(UDPSocket& aSocket);

		virtual ~BaseConnection() {}
		
		UDPSocket* GetSocket();
	
		void Update(float aDeltatime);

		MessageID_t Peek();
		bool ReadNextMessage(NetMessage& aMsg);
		void ClearMessages();
		bool HasMessages() const { return !myReceivedMessages.Empty(); }

	protected:
		template<class NetMessageType>
		void SendOrEnqueue(NetMessageType& aNetMessage, const Address& aAddress);
		
		void Clear();

		UDPSocket*				mySocket = nullptr;
		NetMessageQueue<4096>	myReceivedMessages;

	private:
		virtual void OnReceivedMessage(char recvBuffer[Constants::MAX_BUFFER_SIZE], const Address& aAddress) = 0;

		void UpdateReliableMessageQueue(float aDeltatime);

		ReliableNetMessageQueue	myReliableNetMessageQueue;
	};

	template<class NetMessageType>
	inline void BaseConnection::SendOrEnqueue(NetMessageType& aNetMessage, const Address& aAddress)
	{
		static_assert(std::is_base_of_v<NetMessage, NetMessageType>, "NetMessageType must derive from NetMessage");
		if constexpr (std::is_base_of_v<ReliableNetMessage, NetMessageType>)
		{
			myReliableNetMessageQueue.Enqueue(
				aNetMessage, 
				aAddress, 
				Constants::MAX_RESEND_ATTEMPTS, 
				Constants::RESEND_WAIT_TIME_MS);
		}
		else
		{
			mySocket->Send(aNetMessage, aAddress);
		}
	}
}

