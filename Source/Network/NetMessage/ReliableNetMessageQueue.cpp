#include "pch.h"
#include "ReliableNetMessageQueue.h"

namespace Network
{
	void ReliableNetMessageQueue::Send(UDPSocket& aSocket)
	{
		using namespace std::literals;
		char sendBuffer[Constants::MAX_BUFFER_SIZE]{};
		// Message count
		sendBuffer[0] = 1;

		std::vector<int> timedOutindices;

		auto now = std::chrono::steady_clock::now();
		for (int i = 0; i < myQueueItems.size(); i++)
		{
			ReliableMessageQueueItem& item = myQueueItems[i];
			if (item.myResendAttempts <= 0)
			{
				timedOutindices.push_back(i);
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
		for (int i = static_cast<int>(timedOutindices.size()) - 1; i >= 0; i--)
		{
			int indexToRemove = timedOutindices[i];
			
			myTimedOutMessages.push_back(*myQueueItems[indexToRemove].myMessage);
			
			myQueueItems[indexToRemove] = myQueueItems.back();
			myQueueItems.pop_back();
		}
	}

	std::shared_ptr<ReliableNetMessage> ReliableNetMessageQueue::RemoveMessage(unsigned int aSequence)
	{
		std::shared_ptr<ReliableNetMessage> netMessage = nullptr;
		for (size_t i = 0; i < myQueueItems.size(); i++)
		{
			if (myQueueItems[i].mySequenceNr == aSequence)
			{
				netMessage = myQueueItems[i].myMessage;
				myQueueItems.erase(myQueueItems.begin() + i);
			}
		}
		return netMessage;
	}

	inline void ReliableNetMessageQueue::Clear()
	{
		myQueueItems.clear();
		myTimedOutMessages.clear();
	}
}
