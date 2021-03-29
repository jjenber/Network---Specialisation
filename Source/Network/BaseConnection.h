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
		BaseConnection(UDPSocket& aSocket);
		virtual ~BaseConnection() {}
		
		UDPSocket& GetSocket();
	
		void Update();

		MessageID_t Peek();
		bool ReadNextMessage(NetMessage& aMsg);
		void ClearMessages();

	protected:
		template<class NetMessageType>
		void SendOrEnqueue(const NetMessageType& aNetMessage, const Address& aAddress);
		
		UDPSocket&				mySocket;
		NetMessageQueue<1024>	myReceivedMessages;
	private:
		virtual void OnReceivedMessage(char recvBuffer[Constants::MAX_BUFFER_SIZE], const Address& aAddress) = 0;

		void UpdateReliableMessageQueue();

		ReliableNetMessageQueue	myReliableNetMessageQueue;
	};

	template<class NetMessageType>
	inline void BaseConnection::SendOrEnqueue(const NetMessageType& aNetMessage, const Address& aAddress)
	{
		static_assert(std::is_base_of_v<NetMessage, NetMessageType>, "NetMessageType must derive from NetMessage");
		if constexpr (std::is_base_of_v<ReliableNetMessage, NetMessageType>)
		{
			myReliableNetMessageQueue.Enqueue(aNetMessage, aAddress, 10, 100.f);
		}
		else
		{
			mySocket.Send(aNetMessage, aAddress);
		}
	}
}

