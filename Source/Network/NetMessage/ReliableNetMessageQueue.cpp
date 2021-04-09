#include "pch.h"
#include "ReliableNetMessageQueue.h"

namespace Network
{
	void ReliableNetMessageQueue::Send(UDPSocket& aSocket)
	{
		using namespace std::literals;
		char sendBuffer[Constants::MAX_BUFFER_SIZE]{};
		ZeroMemory(sendBuffer, Constants::MAX_BUFFER_SIZE);
		
		// Message count
		sendBuffer[0] = 1;

		std::vector<int> timedOutindices;

		auto now = std::chrono::steady_clock::now();
		for (int i = 0; i < myQueueItems.size(); i++)
		{
			auto& item = myQueueItems[i];
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
				ReliableNetMessage* itemMsg = static_cast<ReliableNetMessage*>(item.myMessage);
				memcpy(sendBuffer + 1, &itemMsg->myMessageID, itemMsg->mySize);
				aSocket.Send(sendBuffer, itemMsg->mySize + 1, item.myDestinationAddress);
			}
		}

		// Remove the timed out messages from the item queue.
		for (int i = static_cast<int>(timedOutindices.size()) - 1; i >= 0; i--)
		{
			int indexToRemove = timedOutindices[i];
			myQueueItems[indexToRemove].myDeleter();
			myQueueItems[indexToRemove] = myQueueItems.back();
			myQueueItems.pop_back();
		}
	}

	void ReliableNetMessageQueue::RemoveMessage(unsigned int aSequence)
	{
		for (size_t i = 0; i < myQueueItems.size(); i++)
		{
			if (myQueueItems[i].mySequenceNr == aSequence)
			{
				myQueueItems[i].myDeleter();
				myQueueItems.erase(myQueueItems.begin() + i);
			}
		}
	}

	inline void ReliableNetMessageQueue::Clear()
	{
		myQueueItems.clear();
		myAckedSequenceCache.clear();
	}

	bool ReliableNetMessageQueue::HasReceivedPreviously(const Address& aAddress, unsigned short aSequenceNr)
	{
		auto& ackTimestamps = myAckedSequenceCache[aAddress.ToString()];
		bool alreadyReceived = ackTimestamps.find(aSequenceNr) != ackTimestamps.end();
		if (!alreadyReceived)
		{
			ackTimestamps[aSequenceNr] = 0.f;
		}
		return alreadyReceived;
	}
	void ReliableNetMessageQueue::ClearReceivedSequenceCache(float aDeltatime, float aTimeBeforeRemove)
	{
		for (auto& [addr, timestamps] : myAckedSequenceCache)
		{
			std::vector<unsigned short> timedOut;
			for (auto&& [seq, timer] : timestamps)
			{
				if (timer >= aTimeBeforeRemove)
				{
					timedOut.push_back(seq);
				}
			}
			for (const auto& seq : timedOut)
			{
				timestamps.erase(seq);
			}
		}
	}
}
