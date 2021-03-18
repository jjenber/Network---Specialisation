#include "pch.h"
#include "Server.h"
#include <iostream>

void Network::Server::Startup()
{
	myConnectedClientsCount = 0;
	myUDPSocket.BindToLocal();
	myUDPSocket.SetBlocking(false);
}

int Network::Server::GetNextFreeConnectionSlot()
{
	for (int i = 0; i < Constants::MAX_CLIENT_COUNT; i++)
	{
		if (!myConnectedClients[i])
		{
			return i;
		}
	}
	return -1;
}

int Network::Server::FindConnectedClientSlot(const Address& aAddress)
{
	for (int i = 0; i < Constants::MAX_CLIENT_COUNT; i++)
	{
		if (myConnectedClients[i] && myClientAddresses[i] == aAddress)
		{
			return i;
		}
	}
	return NoClientFound;
}

void Network::Server::ReceiveIncomingMessages()
{
	Address fromAddress;
	char recvBuf[Constants::MAX_BUFFER_SIZE]{};

	while (myUDPSocket.Receive(recvBuf, fromAddress))
	{
		myReceivedMessages.EnqueueReceived(recvBuf);
		CheckNewConnection(myReceivedMessages.Peek(), fromAddress);
	}

	while (!myReceivedMessages.Empty())
	{
		Decode(myReceivedMessages.Peek());
	}
}

void Network::Server::Decode(MessageID_t aNetMessageID)
{
	switch (aNetMessageID)
	{
		case eNETMESSAGE_HANDSHAKE:
		{
			ReliableNetMessage msg;
			myReceivedMessages.Dequeue(msg);
			
			break;
		}
	default:

		break;
	}
}

void Network::Server::CheckNewConnection(MessageID_t aMessageID, const Address& aAddress)
{
	switch (aMessageID)
	{
	case eNETMESSAGE_HANDSHAKE:
	{
		ReliableNetMessage msg;
		myReceivedMessages.Dequeue(msg);
		msg.GetSequence();
		break;
	}
	default:
		break;
	}
}
