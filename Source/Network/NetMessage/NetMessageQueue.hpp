#pragma once
#include "NetMessage.h"
#include <array>
#include <queue>
#include <string>
#include <cassert>

namespace Network
{
	constexpr unsigned int UDP_HEADER_SIZE = 8;

	template <size_t _SIZE = 512>
	class NetMessageQueue
	{
	public:
		NetMessageQueue() = default;
		NetMessageQueue(const NetMessageQueue& aCopy);
		NetMessageQueue& operator=(const NetMessageQueue& aCopy);

		void Enqueue(const NetMessage& aMessage);
		void EnqueueReceivedBuffer(char aDataBuffer[Constants::MAX_BUFFER_SIZE]);

		/// Returns the type of the next message without modifying the queue.
		eNetMessageID Peek() const;
		const unsigned short PeekReliableSequence() const;

		/// Fills the provided message with data from the queue and "removes" it from the queue. The caller allocates memory for the message by checking type using Peek().
		void Dequeue(NetMessage& aMessageOut);

		/// Returns a buffer with the next dequeued message. Prepends a 1 to first position in the buffer as message count.
		void Dequeue(char aBufferOut[Constants::MAX_BUFFER_SIZE], size_t& aOutLen);

		size_t GetLength() const { return myBack; }

		bool Empty() const { return myFront == myBack; }
		void Clear() { myFront = 0; myBack = 0; ZeroMemory(myDataBuffer.data(), _SIZE); }

		// Fills up the send buffer with messages until queue is empty or MAX_BUFFER_SIZE is reached. Returns false if no messages are queued.
		bool GetNextMessageBufferToSend(char aBufferOut[Constants::MAX_BUFFER_SIZE], size_t& aBufferLen);

		constexpr size_t Capacity() const { return _SIZE; }

	private:
		size_t myFront = 0;
		size_t myBack = 0;

		std::array<char, _SIZE> myDataBuffer = std::array<char, _SIZE>();
	};

	template<size_t _SIZE>
	inline NetMessageQueue<_SIZE>::NetMessageQueue(const NetMessageQueue& aCopy)
	{
		myBack = aCopy.myBack;
		myFront = aCopy.myFront;
		memcpy(&myDataBuffer[0], aCopy.myDataBuffer.data(), myBack);
	}

	template<size_t _SIZE>
	inline NetMessageQueue<_SIZE>& NetMessageQueue<_SIZE>::operator=(const NetMessageQueue<_SIZE>& aCopy)
	{
		myBack = aCopy.myBack;
		myFront = aCopy.myFront;
		memcpy(&myDataBuffer[0], aCopy.myDataBuffer.data(), myBack);
		return *this;
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Enqueue(const NetMessage& aMessage)
	{
		assert(myBack + aMessage.mySize < _SIZE && "ERROR: No room for the message!");

		// Copying header data and move length
		memcpy(myDataBuffer.data() + myBack, &aMessage.myMessageID, aMessage.mySize);
		myBack += aMessage.mySize;
	}

	template<size_t _SIZE>
	inline eNetMessageID NetMessageQueue<_SIZE>::Peek() const
	{
		eNetMessageID type = eNetMessageID::eNETMESSAGE_NONE;
		if (!Empty())
		{
			memcpy(&type, myDataBuffer.data() + myFront, sizeof(eNetMessageID));
		}
		return type;
	}

	template<size_t _SIZE>
	inline const unsigned short NetMessageQueue<_SIZE>::PeekReliableSequence() const
	{
		unsigned short ID = -1;
		if (!Empty())
		{
			memcpy(&ID, myDataBuffer.data() + myFront + NetMessage::Size(), sizeof(unsigned short));
		}
		return ID;
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Dequeue(NetMessage& aMessageOut)
	{
		// Copy the header from the front of the queue.
		memcpy(&aMessageOut.myMessageID, myDataBuffer.data() + myFront, NetMessage::Size());
		myFront += NetMessage::Size();
		
		int msgBodySize = aMessageOut.mySize - NetMessage::Size();

		// Copy potential body trailing the header.
		if (msgBodySize > 0)
		{
			memcpy(&aMessageOut.myMessageID + NetMessage::Size(), myDataBuffer.data() + myFront, msgBodySize);
		}

		myFront += msgBodySize;
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Dequeue(char aBufferOut[Constants::MAX_BUFFER_SIZE], size_t& aOutLen)
	{
		// Message count
		aBufferOut[0] = MessageID_t(1);

		// Copy the header.
		NetMessage header;
		memcpy(&header, myDataBuffer.data() + myFront, NetMessage::Size());
		memcpy(aBufferOut + MessageID_t(1), myDataBuffer.data() + myFront, header.mySize);
		
		// Move the queue.
		myFront += header.mySize;
		aOutLen  = static_cast<size_t>(header.mySize) + MessageID_t(1);
	}

	template<size_t _SIZE>
	inline bool NetMessageQueue<_SIZE>::GetNextMessageBufferToSend(char aBufferOut[Constants::MAX_BUFFER_SIZE], size_t& aBufferLen)
	{
		// Make room for leading char to denote total number of messages copied to the buffer.
		size_t bufferLen = 1;
		memset(aBufferOut, 0, Constants::MAX_BUFFER_SIZE);

		int messageCount = 0;
		NetMessage header;
		while (!Empty() && bufferLen < Constants::MAX_BUFFER_SIZE)
		{
			memcpy(&header, myDataBuffer.data() + myFront, NetMessage::Size());
			if (header.mySize + bufferLen > Constants::MAX_BUFFER_SIZE)
			{
				// Not enough room in the buffer for the next message.
				break;
			}

			// Copy the message from the queue to the char buffer
			memcpy(aBufferOut + bufferLen, myDataBuffer.data( ) + myFront, header.mySize);
			myFront   += header.mySize;
			bufferLen += header.mySize;
			messageCount += 1;
		}

		aBufferOut[0] = static_cast<char>(messageCount);
		aBufferLen = bufferLen;
		return messageCount > 0;
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::EnqueueReceivedBuffer(char aDataBuffer[Constants::MAX_BUFFER_SIZE])
	{
		int messageCount = static_cast<int>(aDataBuffer[0]);
		size_t readPos = 1;
		NetMessage header;
		for (int i = 0; i < messageCount; i++)
		{
			// Copy from the buffer to a header to get size.
			memcpy(&header.myMessageID, aDataBuffer + readPos, NetMessage::Size());
			
			assert(myBack + header.mySize < _SIZE && "Buffer overflow. No room for databuffer.");
			assert(header.myMessageID != eNETMESSAGE_NONE && "Failed to decode message.");

			// Enqueue 
			memcpy(myDataBuffer.data() + myBack, aDataBuffer + readPos, header.mySize);
			myBack  += header.mySize;
			readPos += header.mySize;
		}
	}
}