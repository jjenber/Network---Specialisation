#include "pch.h"
#include "Server.h"
#include <iostream>

void Network::Server::Startup()
{
	myTime = 0;
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
	myTime += aDeltatime;
	ReceiveIncomingMessages();

	SendHeartbeat();
}

void Network::Server::SendHeartbeat()
{
	for (int i = 0; i < myConnectedClients.size(); i++)
	{
		if (myConnectedClients.test(i))
		{

		}
	}
}

void Network::Server::Decode(MessageID_t aNetMessageID)
{

}

void Network::Server::CheckNewConnection(MessageID_t aMessageID, const Address& aAddress, int aClientSlot)
{
	if (aMessageID != eNETMESSAGE_HANDSHAKE)
	{ 
		return; 
	}

	if (aClientSlot == ClientNotFound && HasFreeSlot())
	{
		// Establish a new connection.
		aClientSlot = GetNextFreeClientSlot();
		myClientAddresses[aClientSlot] = aAddress;
		myConnectedClients[aClientSlot] = true;
		myClientData[aClientSlot].connectionTime = myTime;
		myConnectedClientsCount++;

		std::cout << "New client " << aAddress.ToString() << std::endl;
	}
	else if (aClientSlot == ClientNotFound && !HasFreeSlot())
	{
		// Server Full
		NetMessage serverFull(eNETMESSAGE_SERVER_FULL);
		myUDPSocket.Send(serverFull, aAddress);
		std::cout << "New client tried to connect but the server is full." << aAddress.ToString() << std::endl;
	}
	else
	{
		HandshakeMessage msg;
		myReceivedMessages.Dequeue(msg);
		msg.myClientSlot = aClientSlot;
		myUDPSocket.Send(msg, aAddress);
		std::cout << "Responding to handshake" << std::endl;
	}
}
