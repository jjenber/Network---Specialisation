#include "pch.h"
#include "Server.h"
#include <iostream>

void Network::Server::Startup()
{
	myConnectedClientsCount = 0;
	myUDPSocket.BindToLocal();
	myUDPSocket.SetBlocking(false);
}

int Network::Server::GetNextFreeClientSlot()
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
	return ClientNotFound;
}

void Network::Server::ReceiveIncomingMessages()
{
	Address fromAddress;
	char recvBuf[Constants::MAX_BUFFER_SIZE]{};
	int clientSlot = Server::ClientNotFound;

	while (myUDPSocket.Receive(recvBuf, fromAddress))
	{
		myReceivedMessages.EnqueueReceived(recvBuf);
		int clientSlot = FindConnectedClientSlot(fromAddress);
		CheckNewConnection(myReceivedMessages.Peek(), fromAddress, clientSlot);
	}

	while (!myReceivedMessages.Empty())
	{
		Decode(myReceivedMessages.Peek());
	}
}

void Network::Server::Update(const float aDeltatime)
{
}

void Network::Server::SendHeartbeat()
{
}

void Network::Server::Decode(MessageID_t aNetMessageID)
{

}

void Network::Server::CheckNewConnection(MessageID_t aMessageID, const Address& aAddress, int aClientSlot)
{
	if (aMessageID != eNETMESSAGE_HANDSHAKE) { return; }

	if (aClientSlot == ClientNotFound)
	{
		aClientSlot = GetNextFreeClientSlot();
		myClientAddresses[aClientSlot] = aAddress;
		myConnectedClients[aClientSlot] = true;
		myClientData[aClientSlot] = ClientData{};

		std::cout << "New client " << aAddress.ToString() << std::endl;
	}

	HandshakeMessage msg;
	myReceivedMessages.Dequeue(msg);
	msg.myClientSlot = aClientSlot;
	myUDPSocket.Send(msg, aAddress);
	std::cout << "Responding to handshake" << std::endl;
}
