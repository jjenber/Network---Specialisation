#include "pch.h"
#include "Server.h"
#include <iostream>

void Network::Server::Startup()
{
	myUDPSocket.BindToLocal();
	myUDPSocket.SetBlocking(false);
}

int Network::Server::GetNextFreeConnectionSlot()
{
	for (int i = 0; i < Constants::MAX_CLIENT_COUNT; i++)
	{
		if (!myUDPConnections[i].myConnected)
		{
			return i;
		}
	}
	return -1;
}

void Network::Server::ReceiveIncomingMessages()
{
	Address addr;
	char recvBuf[Constants::MAX_BUFFER_SIZE];
	int length;

	while (myUDPSocket.Receive(recvBuf, Constants::MAX_BUFFER_SIZE, addr, length))
	{
		myReceivedMessages.EnqueueFromBuffer(recvBuf, length);
	}

	while (!myReceivedMessages.Empty())
	{
		MessageID_t msgID = myReceivedMessages.Peek();
		if (msgID > eNetMessageID::eNETMESSAGE_RELIABLE_ID)
		{
			DecodeReliable(msgID);
		}
		else
		{
			Decode(msgID);
		}
	}
}

void Network::Server::Decode(MessageID_t aNetMessageID)
{
	
}

void Network::Server::DecodeReliable(MessageID_t aNetMessageID)
{
	switch (aNetMessageID)
	{
		case eNETMESSAGE_HANDSHAKE:
		{
			ReliableNetMessage msg;
			myReceivedMessages.Dequeue(msg);
			std::cout << "Received: " << msg.GetMessageType() << std::endl;
			break;
		}
	default:
		break;
	}
}
