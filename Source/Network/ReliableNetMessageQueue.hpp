#pragma once
#include "NetMessageQueue.hpp"
#include <chrono>
#include <vector>
#include <iostream>

namespace Network
{
	template <size_t _SIZE>
	class ReliableNetMessageQueue
	{
	public:
		template<class T>
		void Enqueue(T& aMessage, Address aDestinationAddress);
		void Send(UDPSocket& aSocket);
	private:
		
		// Wraps a reliable message with meta data to keep track of resends.
		struct ReliableMessageQueueItem
		{
			std::chrono::steady_clock::time_point myTimestamp;
			ReliableNetMessage* myMessage = nullptr;
			Address myDestinationAddress;
			
			float myResendTimer  = Constants::RESEND_WAIT_TIME_MS;
			int myResendAttempts = 1;
		};

		std::vector<ReliableMessageQueueItem> myQueueItems;
		NetMessageQueue<_SIZE> myQueue;

		unsigned short myReliableAckID = 0;
	};

	template<size_t _SIZE>
	template<class T>
	inline void ReliableNetMessageQueue<_SIZE>::Enqueue(T& aMessage, Address aDestinationAddress)
	{
		static_assert(std::is_base_of_v<ReliableNetMessage, T>, "T must derive from ReliableNetMessage");

		ReliableMessageQueueItem item;
		item.myMessage = reinterpret_cast<ReliableNetMessage*>(new T(aMessage));
		item.myMessage->myAckID = myReliableAckID++;
		item.myDestinationAddress = aDestinationAddress;
		item.myTimestamp = std::chrono::steady_clock::now();
		myQueueItems.push_back(item);
	}

	template<size_t _SIZE>
	inline void ReliableNetMessageQueue<_SIZE>::Send(UDPSocket& aSocket)
	{
		using namespace std::literals;
		char sendBuffer[Constants::MAX_BUFFER_SIZE];
		// Message count
		sendBuffer[0] = 1;

		std::vector<int> timedOutMessageIndices;

		auto now = std::chrono::steady_clock::now();
		for (int i = 0; i < myQueueItems.size(); i++)
		{
			auto& item = myQueueItems[i];
			if (item.myResendAttempts > Constants::MAX_RESEND_ATTEMPTS)
			{
				timedOutMessageIndices.push_back(i);
				continue;
			}

			float timeSinceLastSend = std::chrono::duration_cast<std::chrono::milliseconds>(now - item.myTimestamp).count();
			if (timeSinceLastSend >= Constants::RESEND_WAIT_TIME_MS)
			{
				item.myTimestamp = now;
				item.myResendAttempts++;
				memcpy(sendBuffer, *item.myMessage, sizeitem.myMessage->myBodySize);
				aSocket.Send(sendBuffer, item.myMessage->myBodySize, item.myDestinationAddress);
				std::cout << "Sending reliable message." << std::endl;
			}
		}

		// Remove the timed out messages from the item queue.
		for (int i = timedOutMessageIndices.size() - 1; i >= 0; i--)
		{
			int indexToRemove = timedOutMessageIndices[i];
			delete myQueueItems[indexToRemove].myMessage;
			myQueueItems[indexToRemove] = myQueueItems.back();
			myQueueItems.pop_back();
			std::cout << "Message timed out" << std::endl;
		}
	}
}
