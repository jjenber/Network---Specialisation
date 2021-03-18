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
		void Enqueue(T& aMessage, Address aDestinationAddress, int aResendAttempts, float aResendWaitTime);
		void Send(UDPSocket& aSocket);
		void RemoveMessage(unsigned int aSequence);
		void Clear();
	private:
		
		// Wraps a reliable message with meta data to keep track of resends.
		struct ReliableMessageQueueItem
		{
			ReliableMessageQueueItem(int aResendAttempts, float aResendWaitTime)
				: myResendAttempts(aResendAttempts), myResendTimer(aResendWaitTime), myResendWaitTime(aResendWaitTime) {};

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

	void ReliableNetMessageQueue::Enqueue(T& aMessage, Address aDestinationAddress, int aResendAttempts, float aResendWaitTime)
	{
		static_assert(std::is_base_of_v<ReliableNetMessage, T>, "T must derive from ReliableNetMessage");

		ReliableMessageQueueItem item(aResendAttempts, aResendWaitTime);
		item.myMessage               = std::make_shared<T>(aMessage);
		item.myMessage->mySize       = sizeof(T) - sizeof(void*);
		item.myMessage->mySequenceNr = mySequenceNr++;
		item.myDestinationAddress    = aDestinationAddress;
		item.myTimestamp             = std::chrono::steady_clock::now();
		myQueueItems.push_back(item);
	}

	void ReliableNetMessageQueue::Send(UDPSocket& aSocket)
	{
		using namespace std::literals;
		char sendBuffer[Constants::MAX_BUFFER_SIZE]{};
		// Message count
		sendBuffer[0] = 1;

		std::vector<int> timedOutMessageIndices;

		auto now = std::chrono::steady_clock::now();
		for (int i = 0; i < myQueueItems.size(); i++)
		{
			ReliableMessageQueueItem& item = myQueueItems[i];
			if (item.myResendAttempts <= 0)
			{
				timedOutMessageIndices.push_back(i);
				continue;
			}

			auto timeSinceLastSend = std::chrono::duration_cast<std::chrono::milliseconds>(now - item.myTimestamp).count();
			if (timeSinceLastSend >= Constants::RESEND_WAIT_TIME_MS)
			{
				item.myTimestamp = now;
				item.myResendAttempts--;

				// Copy over the message and offset for the count.
				memcpy(sendBuffer + 1, &item.myMessage->myMessageID, item.myMessage->mySize);
				aSocket.Send(sendBuffer, item.myMessage->mySize, item.myDestinationAddress);
				
				std::cout << "Sending reliable message." << std::endl;
			}
		}

		// Remove the timed out messages from the item queue.
		for (int i = static_cast<int>(timedOutMessageIndices.size()) - 1; i >= 0; i--)
		{
			int indexToRemove = timedOutMessageIndices[i];
			delete myQueueItems[indexToRemove].myMessage;

			myQueueItems[indexToRemove] = myQueueItems.back();
			myQueueItems.pop_back();
			std::cout << "Message timed out" << std::endl;
		}
	}

	inline void ReliableNetMessageQueue::Clear()
	{
		myQueueItems.clear();
	}
}
