#pragma once
#include "NetMessage.h"
#include "Address\Address.h"
#include "Socket\UDPSocket.h"
#include <chrono>
#include <vector>
#include <iostream>

namespace Network
{
	class ReliableNetMessageQueue
	{
	public:
		template<class T>
		void Enqueue(T& aMessage, const Address& aDestinationAddress, int aResendAttempts, float aResendWaitTime);
		void Send(UDPSocket& aSocket);
		void RemoveMessage(unsigned int aSequence);
		void Clear();
	private:
		
		// Wraps a reliable message with meta data to keep track of resends.
		struct ReliableMessageQueueItem
		{
			ReliableMessageQueueItem(int aResendAttempts, float aResendWaitTime)
				: myResendAttempts(aResendAttempts), myResendTimer(aResendWaitTime), myResendWaitTime(aResendWaitTime), myDestinationAddress() {};

			std::chrono::steady_clock::time_point myTimestamp;
			std::shared_ptr<ReliableNetMessage> myMessage;
			Address myDestinationAddress;
			
			float myResendTimer;
			float myResendWaitTime;
			int myResendAttempts;
		};

		std::vector<ReliableMessageQueueItem> myQueueItems;
		unsigned short mySequenceNr = 0;
	};
	template <class T>
	void ReliableNetMessageQueue::Enqueue(T& aMessage, const Address& aDestinationAddress, int aResendAttempts, float aResendWaitTime)
	{
		static_assert(std::is_base_of_v<ReliableNetMessage, T>, "T must derive from ReliableNetMessage");

		ReliableMessageQueueItem item(aResendAttempts, aResendWaitTime);
		item.myMessage = std::make_shared<T>(aMessage);
		item.myMessage->mySize = sizeof(T) - sizeof(void*);
		item.myMessage->mySequenceNr = mySequenceNr++;
		item.myDestinationAddress = aDestinationAddress;
		item.myTimestamp = std::chrono::steady_clock::now();
		myQueueItems.push_back(item);
	}

	
}
