#include "pch.h"
#include "BaseConnection.h"

namespace Network
{
	BaseConnection::BaseConnection(UDPSocket& aSocket) :
		mySocket(aSocket)
	{
		mySocket.SetBlocking(false);
	}

	UDPSocket& Network::BaseConnection::GetSocket()
	{
		return mySocket;
	}

	void BaseConnection::Update(float aDeltatime)
	{
		UpdateReliableMessageQueue(aDeltatime);

		Address fromAddress;
		char recvBuf[Constants::MAX_BUFFER_SIZE]{};

		while (mySocket.Receive(recvBuf, fromAddress))
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
				memcpy(&sequence, recvBuf + 1 + NetMessage::Size(), sizeof(unsigned short));
				AcknowledgementMessage ack(msgID, sequence);
				assert(ack.mySequenceNr != USHRT_MAX);
				mySocket.Send(ack, fromAddress);

				if (!myReliableNetMessageQueue.HasReceivedPreviously(fromAddress, ack.mySequenceNr))
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

	void BaseConnection::UpdateReliableMessageQueue(float aDeltatime)
	{
		myReliableNetMessageQueue.Send(mySocket);
		myReliableNetMessageQueue.ClearReceivedSequenceCache(aDeltatime, 20.f);

	}
}
