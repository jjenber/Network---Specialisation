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

	void BaseConnection::Update()
	{
		UpdateReliableMessageQueue();

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
			else
			{
				OnReceivedMessage(recvBuf, fromAddress);
			}
		}
	}

	void BaseConnection::UpdateReliableMessageQueue()
	{
		myReliableNetMessageQueue.Send(mySocket);
		myReliableNetMessageQueue.GetTimedOutMessages().clear();
	}
}
