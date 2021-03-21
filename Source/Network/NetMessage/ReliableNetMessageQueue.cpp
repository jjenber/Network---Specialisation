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
