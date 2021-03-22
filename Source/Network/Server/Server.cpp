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

void Network::Server::DisconnectClient(int aClientSlot)
{
	myConnectedClients[aClientSlot] = false;
	myClientData[aClientSlot] = ClientData{};
	std::cout << "Disconnecting " << aClientSlot << std::endl;
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
	myReceivedMessages.Clear();
}

void Network::Server::Update(const float aDeltatime)
{
	myTime += aDeltatime;
	ReceiveIncomingMessages();
	SendHeartbeat();
}

void Network::Server::SendHeartbeat()
{
	NetMessage heartbeat(eNETMESSAGE_HEARTBEAT);

	for (size_t i = 0; i < Constants::MAX_CLIENT_COUNT; i++)
	{
		if (myConnectedClients.test(i) && ((myTime - myClientData[i].timeLastHeartbeatSent) > 5.f))
		{
			myClientData[i].timeLastHeartbeatSent = myTime;
			myUDPSocket.Send(heartbeat, myClientAddresses[i]);
			std::cout << "Sending heartbeat to " << myClientAddresses[i].ToString() << std::endl;
		}
	}
}

void Network::Server::Decode(MessageID_t aNetMessageID)
{
	switch (aNetMessageID)
	{
	case eNETMESSAGE_HEARTBEAT:
	{
		NetMessage heartbeat;
		myReceivedMessages.Dequeue(heartbeat);
		myClientData[heartbeat.mySenderID].timeLastHeartbeatReceived = myTime;
		std::cout << "Receive heartbeat to " << myClientAddresses[heartbeat.mySenderID].ToString() << std::endl;
		break;
	}
	case eNETMESSAGE_DISCONNECT:
	{
		ReliableNetMessage msg;
		myReceivedMessages.Dequeue(msg);
		AcknowledgementMessage ack(msg.mySequenceNr);
		myUDPSocket.Send(ack, myClientAddresses[msg.mySenderID]);
		DisconnectClient(msg.mySenderID);
		break;
	}
	default:
		std::cout << "Unhandled message: " << (int)aNetMessageID << std::endl;
		break;
	}
}

void Network::Server::CheckNewConnection(MessageID_t aMessageID, const Address& aAddress, int aClientSlot)
{
	if (aMessageID != eNETMESSAGE_HANDSHAKE)
	{ 
		return; 
	}

	HandshakeMessage receivedHandshake;
	myReceivedMessages.Dequeue(receivedHandshake);

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
		return;
	}

	receivedHandshake.myClientSlot = aClientSlot;
	myUDPSocket.Send(receivedHandshake, aAddress);
	std::cout << "Responding to handshake" << std::endl;
}
