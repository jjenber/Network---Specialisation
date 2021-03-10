#pragma once
#include "NetMessage.hpp"
#include <array>
#include <queue>
#include <string>

///
/// TODO: Make it circular
///
namespace Networking
{
	template <size_t _SIZE = 512>
	class NetMessageQueue
	{
	public:
		void Enqueue(const NetMessage& aMessage);
		void Enqueue(const unsigned short aSenderID, const std::string& aMessage);
		void EnqueueFromBuffer(char aReceivedBuffer[BUFFER_SIZE]);

		/// Returns the type of the next message without modifying the queue.
		Messages::eMessage Peek( ) const;
		const unsigned short PeekGuaranteeID( ) const;

		/// Fills the provided message with data from the queue and "removes" it from the queue.
		void Dequeue(NetMessage& aMessageOut);

		/// Returns a buffer with the next dequeued message. Prepends a 1 to first position in the buffer as message count.
		void Dequeue(char aBufferOut[BUFFER_SIZE], size_t& aOutLen);

		// Returns number of messages that are currently queued.
		size_t GetQueuedCount( ) const { return myDataStrides.size( ); }
		size_t GetLength( ) const { return myLength; }

		bool IsEmpty( ) const { return myFront == myLength; }
		void Clear( ) { myLength = 0; myFront = 0; myDataStrides = std::queue<size_t>( ); }

		// Fills up the send buffer with messages until queue is empty or BUFFER_SIZE is reached. Returns false if no messages are queued.
		bool GetNextMessageBufferToSend(char aBufferOut[BUFFER_SIZE], size_t& aBufferLen);

		constexpr size_t Capacity() const { return _SIZE; }

	private:
		size_t myLength = 0;
		size_t myFront = 0;

		std::array<char, _SIZE> myDataBuffer = std::array<char, _SIZE>( );
		std::queue<size_t>      myDataStrides;
	};

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Enqueue(const NetMessage& aMessage)
	{
		size_t totalMessageSize = aMessage.GetBodySize( ) + Messages::NETMESSAGE_HEADER_SIZE;
		assert(myLength + totalMessageSize < _SIZE && "ERROR: No room for the message!");

		// Copying header data and move length
		memcpy(myDataBuffer.data( ) + myLength, &aMessage.myMessageType, Messages::NETMESSAGE_HEADER_SIZE);
		myLength += Messages::NETMESSAGE_HEADER_SIZE;

		// Copying data data and move length
		memcpy(myDataBuffer.data( ) + myLength, &aMessage + 1, aMessage.GetBodySize( ));

		myLength += aMessage.GetBodySize( );
		myDataStrides.push(aMessage.GetBodySize( ));
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Enqueue(const unsigned short aSenderID, const std::string& aMessage)
	{
		size_t textLength = aMessage.length( );
		if (textLength < 64)
		{
			Messages::TextNetMessage64 txtMsg;
			strcpy_s(txtMsg.myText, aMessage.size( ) + 1, aMessage.data( ));
			txtMsg.mySenderID = aSenderID;
			Enqueue(txtMsg);
		}
		else if (textLength < 128)
		{
			Messages::TextNetMessage128 txtMsg;
			strcpy_s(txtMsg.myText, 128, aMessage.data( ));
			txtMsg.mySenderID = aSenderID;
			Enqueue(txtMsg);
		}
		else if (textLength < 252)
		{
			Messages::TextNetMessage252 txtMsg;
			strcpy_s(txtMsg.myText, 252, aMessage.data( ));
			txtMsg.mySenderID = aSenderID;
			Enqueue(txtMsg);
		}
		else
		{
			assert(false && "A Text eMessage that was equal or larger than 252 characters tried to be enqueue");
		}
	}

	template<size_t _SIZE>
	inline eMessage NetMessageQueue<_SIZE>::Peek( ) const
	{
		Messages::eMessage type = Messages::eMESSAGE_NONE;
		if (!IsEmpty( ))
		{
			memcpy(&type, myDataBuffer.data( ) + myFront, sizeof(Messages::eMessage));
		}
		return type;
	}

	template<size_t _SIZE>
	inline const unsigned short NetMessageQueue<_SIZE>::PeekGuaranteeID( ) const
	{
		unsigned short ID = -1;
		if (!IsEmpty( ))
		{
			memcpy(&ID, myDataBuffer.data( ) + myFront + Messages::NETMESSAGE_HEADER_SIZE, sizeof(unsigned short));
		}
		return ID;
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Dequeue(Messages::NetMessage& aMessageOut)
	{
		// Copy the header with message type and sender id.
		memcpy(&aMessageOut.myMessageType, myDataBuffer.data( ) + myFront, Messages::NETMESSAGE_HEADER_SIZE);
		myFront += Messages::NETMESSAGE_HEADER_SIZE;

		// Copy potential data
		if (aMessageOut.GetBodySize( ) > 0)
		{
			memcpy(&aMessageOut + 1, myDataBuffer.data( ) + myFront, aMessageOut.GetBodySize( ));
		}

		myFront += aMessageOut.GetBodySize( );
		myDataStrides.pop( );
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::Dequeue(char aBufferOut[BUFFER_SIZE], size_t& aOutLen)
	{
		// eMessage count
		constexpr int sizeOfMessageCountType = 1;
		aBufferOut[0] = char(sizeOfMessageCountType);

		// Copy the header with message type and sender id.
		memcpy(aBufferOut + 1, myDataBuffer.data( ) + myFront, Messages::NETMESSAGE_HEADER_SIZE);
		myFront += Messages::NETMESSAGE_HEADER_SIZE;

		// Copy potential data
		const size_t bodySize = myDataStrides.front( );
		if (bodySize > 0)
		{
			memcpy(aBufferOut + Messages::NETMESSAGE_HEADER_SIZE + sizeOfMessageCountType, myDataBuffer.data( ) + myFront, bodySize);
		}

		myFront += bodySize;
		myDataStrides.pop( );
		aOutLen = Messages::NETMESSAGE_HEADER_SIZE + bodySize + sizeOfMessageCountType;
	}

	template<size_t _SIZE>
	inline bool NetMessageQueue<_SIZE>::GetNextMessageBufferToSend(char aBufferOut[BUFFER_SIZE], size_t& aBufferLen)
	{
		// Make room for leading char to denote total number of messages in buffer.
		size_t bufferLen = 1;
		ZeroMemory(aBufferOut, BUFFER_SIZE);

		int messageCount = 0;
		while (!IsEmpty( ) && bufferLen < BUFFER_SIZE)
		{
			size_t dataStride = myDataStrides.empty( ) ? 0 : myDataStrides.front( );
			if (dataStride + Messages::NETMESSAGE_HEADER_SIZE + bufferLen > BUFFER_SIZE)
			{
				// Not enough room for next message in Queue.
				break;
			}

			// Copy in the header data
			memcpy(aBufferOut + bufferLen, myDataBuffer.data( ) + myFront, Messages::NETMESSAGE_HEADER_SIZE);
			myFront   += Messages::NETMESSAGE_HEADER_SIZE;
			bufferLen += Messages::NETMESSAGE_HEADER_SIZE;

			// Copy in the data data
			memcpy(aBufferOut + bufferLen, myDataBuffer.data( ) + myFront, dataStride);
			myFront += dataStride;
			bufferLen += dataStride;

			myDataStrides.pop( );
			messageCount += 1;
		}

		aBufferOut[0] = static_cast<char>(messageCount);
		aBufferLen = bufferLen;
		return messageCount > 0;
	}

	template<size_t _SIZE>
	inline void NetMessageQueue<_SIZE>::EnqueueFromBuffer(char aReceivedBuffer[BUFFER_SIZE])
	{
		char count;
		memcpy(&count, &aReceivedBuffer[0], 1);
		int messageCount = static_cast<int>(count);

		size_t readPos = 1;
		for (int i = 0; i < messageCount; i++)
		{
			Messages::eMessage type;
			memcpy(&type, aReceivedBuffer + readPos, 1);
			size_t bodySize = GetSizeOfMessage(type);

			readPos += Messages::NETMESSAGE_HEADER_SIZE;
			readPos += bodySize;
			myDataStrides.push(bodySize);
		}

		memcpy(myDataBuffer.data( ) + myLength, aReceivedBuffer + 1, readPos - 1);
		myLength += readPos - 1;
	}
}