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
		unsigned short Enqueue(T& aMessage, const Address& aDestinationAddress, int aResendAttempts, float aResendWaitTime);
		
		void Send(UDPSocket& aSocket);
		void RemoveMessage(unsigned int aSequence);
		void Clear();

	private:
		
		// Wraps a reliable message with meta data to keep track of resends.
		struct ReliableMessageQueueItem
		{
			ReliableMessageQueueItem(int aResendAttempts, float aResendWaitTime)
				: myResendAttempts(aResendAttempts), myResendTimer(aResendWaitTime), myResendWaitTime(aResendWaitTime), myDestinationAddress(), myMessage(nullptr) {};

			std::function<void()> myDeleter;
			std::chrono::steady_clock::time_point myTimestamp;
			void* myMessage;
			int myMessageSize = 0;
			Address myDestinationAddress;
			 
			float myResendTimer;
			float myResendWaitTime;
			int myResendAttempts;
			unsigned short mySequenceNr = USHRT_MAX;
		};

		std::vector<ReliableMessageQueueItem> myQueueItems;
		unsigned short mySequenceNr = 0;
	};

	template <class T>
	unsigned short ReliableNetMessageQueue::Enqueue(T& aMessage, const Address& aDestinationAddress, int aResendAttempts, float aResendWaitTime)
	{
		static_assert(std::is_base_of_v<ReliableNetMessage, T>, "T must derive from ReliableNetMessage");
		T* copy = new T(aMessage);

		ReliableNetMessage* header = (ReliableNetMessage*)(copy);
		header->mySequenceNr       = mySequenceNr;
		
		ReliableMessageQueueItem item(aResendAttempts, aResendWaitTime);

		item.myDeleter            = [copy] { delete copy; };
		item.myMessage            = (void*)copy;
		item.myTimestamp          = std::chrono::steady_clock::now();
		item.mySequenceNr         = mySequenceNr++;
		item.myDestinationAddress = aDestinationAddress;
		
		myQueueItems.push_back(item);
		return item.mySequenceNr;
	}
}
