#include "pch.h"
#include "BaseConnection.h"
#include <cassert>

namespace Network
{
	void BaseConnection::Init(UDPSocket& aSocket)
	{
		mySocket = &aSocket;
	}
	UDPSocket* Network::BaseConnection::GetSocket()
	{
		return mySocket;
	}

	void BaseConnection::Update(float aDeltatime)
	{
		myReceivedMessages.Clear(); // Prepare the queue for all incoming messages.

		if (mySocket == nullptr) return;

		UpdateReliableMessageQueue(aDeltatime);

		Address fromAddress;
		char recvBuf[Constants::MAX_BUFFER_SIZE]{};

		while (mySocket->Receive(recvBuf, fromAddress))
		{
			MessageID_t msgID = recvBuf[1];

			if (msgID == eNETMESSAGE_ACKNOWLEDGEMENT)
			{
				AcknowledgementMessage msg;
				memcpy(&msg.myMessageID, recvBuf + 1, msg.mySize);
				myReliableNetMessageQueue.RemoveMessage(msg.mySequenceNr);
			}

			else if (msgID > eNETMESSAGE_RELIABLE_ID)
			{
				uint32_t sequence = 0;
				memcpy(&sequence, recvBuf + 1 + NetMessage::Size(), sizeof(unsigned short));         // Copy out the ack sequence number.
				
				AcknowledgementMessage ack(msgID, sequence);
				assert(ack.mySequenceNr != USHRT_MAX && "Invalid ack value.");
				mySocket->Send(ack, fromAddress);                                                     // Return the ack sequence to the sender.

				if (!myReliableNetMessageQueue.HasReceivedPreviously(fromAddress, ack.mySequenceNr)) // Only enqueue if this is the first time.
				{
					myReceivedMessages.EnqueueReceivedBuffer(recvBuf);
				}
			}

			else
			{
				OnReceivedMessage(recvBuf, fromAddress);
			}
		}
	}

	Network::MessageID_t Network::BaseConnection::Peek()
	{
		return myReceivedMessages.Peek();
	}

	bool Network::BaseConnection::ReadNextMessage(NetMessage& aMsg)
	{
		if (myReceivedMessages.Peek() != eNetMessageID::eNETMESSAGE_NONE)
		{
			myReceivedMessages.Dequeue(aMsg);
			return true;
		}
		return false;
	}

	void BaseConnection::ClearMessages()
	{
		myReceivedMessages.Clear();
	}

	void BaseConnection::Clear()
	{
		myReliableNetMessageQueue.ClearReceivedSequenceCache(1.f, 0);
	}

	void BaseConnection::UpdateReliableMessageQueue(float aDeltatime)
	{
		if (mySocket)
		{
			myReliableNetMessageQueue.Send(*mySocket);
			myReliableNetMessageQueue.ClearReceivedSequenceCache(aDeltatime, 20.f);
		}
	}

	bool BaseConnection::HasUnackedMessages() 
	{
		return myReliableNetMessageQueue.GetQueuedCount() > 0;
	}
}
