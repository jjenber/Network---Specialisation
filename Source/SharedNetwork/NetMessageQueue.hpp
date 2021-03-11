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
		void Enqueue(const NetMessage& aMessage);
		void EnqueueFromBuffer(char aDataBuffer[Constants::MAX_BUFFER_SIZE], int aBufferLength);

		/// Returns the type of the next message without modifying the queue.
		eMessage Peek() const;
		const unsigned short PeekGuaranteeID() const;

		/// Fills the provided message with data from the queue and "removes" it from the queue. The caller allocates memory for the message by checking type using Peek().
		void Dequeue(NetMessage& aMessageOut);

		/// Returns a buffer with the next dequeued message. Prepends a 1 to first position in the buffer as message count.
		void Dequeue(char aBufferOut[Constants::MAX_BUFFER_SIZE], size_t& aOutLen);

		size_t GetLength() const { return myBack; }

		bool IsEmpty() const { return myFront == myBack; }
		void Clear() { myFront = 0; myBack = 0; }

		// Fills up the send buffer with messages until queue is empty or MAX_BUFFER_SIZE is reached. Returns false if no messages are queued.
		bool GetNextMessageBufferToSend(char aBufferOut[Constants::MAX_BUFFER_SIZE], size_t& aBufferLen);

		constexpr size_t Capacity() const { return _SIZE; }

	private:
		size_t myFront = 0;
		size_t myBack = 0;

		std::array<char, _SIZE> myDataBuffer = std::array<char, _SIZE>();
	};

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Enqueue(const NetMessage& aMessage)
	{
		size_t totalMessageSize = aMessage.mySize;
		assert(myBack + totalMessageSize < _SIZE && "ERROR: No room for the message!");

		// Copying header data and move length
		memcpy(myDataBuffer.data() + myBack, &aMessage.myMessageType, totalMessageSize);
		myBack += aMessage.mySize;
	}

	template<size_t _SIZE>
	inline eMessage NetMessageQueue<_SIZE>::Peek() const
	{
		eMessage type = eMessage::eMESSAGE_TYPE_NONE;
		if (!IsEmpty())
		{
			memcpy(&type, myDataBuffer.data() + myFront, sizeof(eMessage));
		}
		return type;
	}

	template<size_t _SIZE>
	inline const unsigned short NetMessageQueue<_SIZE>::PeekGuaranteeID() const
	{
		unsigned short ID = -1;
		if (!IsEmpty())
		{
			memcpy(&ID, myDataBuffer.data() + myFront + sizeof(NetMessage), sizeof(unsigned short));
		}
		return ID;
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Dequeue(NetMessage& aMessageOut)
	{
		// Copy the header from the front of the queue.
		memcpy(&aMessageOut.myMessageType, myDataBuffer.data() + myFront, sizeof(NetMessage));
		myFront += sizeof(NetMessage);
		
		int msgBodySize = aMessageOut.mySize - sizeof(NetMessage);

		// Copy potential body trailing the header.
		if (msgBodySize > 0)
		{
			memcpy(&aMessageOut + 1, myDataBuffer.data() + myFront, msgBodySize);
		}

		myFront += msgBodySize;
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Dequeue(char aBufferOut[Constants::MAX_BUFFER_SIZE], size_t& aOutLen)
	{
		// Message count
		aBufferOut[0] = MessageType_t(1);

		// Copy the header.
		NetMessage header;
		memcpy(&header, myDataBuffer.data() + myFront, sizeof(NetMessage));
		memcpy(aBufferOut + MessageType_t(1), myDataBuffer.data() + myFront, header.mySize);
		
		// Move the queue.
		myFront += header.mySize;
		aOutLen  = header.mySize + MessageType_t(1);
	}

	template<size_t _SIZE>
	inline bool NetMessageQueue<_SIZE>::GetNextMessageBufferToSend(char aBufferOut[Constants::MAX_BUFFER_SIZE], size_t& aBufferLen)
	{
		// Make room for leading char to denote total number of messages copied to the buffer.
		size_t bufferLen = 1;
		ZeroMemory(aBufferOut, Constants::MAX_BUFFER_SIZE);

		int messageCount = 0;
		NetMessage header;
		while (!IsEmpty( ) && bufferLen < Constants::MAX_BUFFER_SIZE)
		{
			memcpy(&header, myDataBuffer.data() + myFront, sizeof(NetMessage));
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
	inline void NetMessageQueue<_SIZE>::EnqueueFromBuffer(char aDataBuffer[Constants::MAX_BUFFER_SIZE], int aBufferLength)
	{
		// The first byte denoting the message count is skipped;
		myBack += aBufferLength - 1;
		assert(myBack < aBufferLength && "Buffer overflow. No room for databuffer.");
		memcpy(myDataBuffer.data() + myBack, aDataBuffer + 1, aBufferLength - 1);
	}
}