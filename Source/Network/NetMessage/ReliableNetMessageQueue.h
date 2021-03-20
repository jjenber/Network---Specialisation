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

	
}
