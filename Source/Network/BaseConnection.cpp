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
		
		for (auto& message : myReliableNetMessageQueue.GetTimedOutMessages())
		{
			if (myCallback)
			{
				myCallback(eMessageStatus::TimedOut, message.myMessageID, message.mySize, &message.myMessageID);
			}
		}
		myReliableNetMessageQueue.GetTimedOutMessages().clear();
	}
}
