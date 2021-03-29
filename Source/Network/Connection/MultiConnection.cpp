#include "pch.h"
#include "MultiConnection.h"

Network::MultiConnection::MultiConnection() : 
	BaseConnection(mySocket),
	myHandshakeID(0),
	myMaxConnections(0)
{
}

void Network::MultiConnection::Init(size_t aMaxConnected, MessageID_t aHandshakeID, OnConnectionCallback_t aOnConnectionCallback)
{
	SetMaxConnections(aMaxConnected);
	SetHandshakeID(aHandshakeID);
	SetOnConnectionCallback(aOnConnectionCallback);
	mySocket.SetBlocking(false);
}

bool Network::MultiConnection::Bind(const Address& aAddress)
{
	return mySocket.Bind(aAddress);
}

bool Network::MultiConnection::Bind(const std::string& aIP, unsigned short aPort)
{
	Address address(aIP, aPort);
	return Bind(address);
}

void Network::MultiConnection::SetMaxConnections(size_t aMaxConnected)
{
	myConnections.resize(aMaxConnected);
}

void Network::MultiConnection::SetHandshakeID(MessageID_t aHandshakeID)
{
	myHandshakeID = aHandshakeID;
}

void Network::MultiConnection::SetOnConnectionCallback(OnConnectionCallback_t aOnConnectionCallback)
{
	myOnConnectionCallback = aOnConnectionCallback;
}

void Network::MultiConnection::OnReceivedMessage(char recvBuffer[Constants::MAX_BUFFER_SIZE], const Address& aAddress)
{
	myReceivedMessages.EnqueueReceivedBuffer(recvBuffer);

	MessageID_t id = myReceivedMessages.Peek();
	if (id == myHandshakeID)
	{
		int connectionSlot = GetConnectionSlot(aAddress);
		HandleHandshakeMessage(connectionSlot, aAddress);
	}
}
void Network::MultiConnection::EstablishNewConnection(int aConnectionSlot, const Address& aAddress)
{
	myConnections[aConnectionSlot].myAddress = aAddress;
	myConnections[aConnectionSlot].myConnectionStatus = eConnectionStatus::Connected;

	if (myOnConnectionCallback)
	{
		myOnConnectionCallback(aAddress, aConnectionSlot);
	}
}

void Network::MultiConnection::HandleHandshakeMessage(int connectionSlot, const Address& aAddress)
{
	NetMessage msg;
	myReceivedMessages.Dequeue(msg);

	int slot = connectionSlot == MultiConnection::ConnectionSlotNotFound ? FindFreeConnectionSlot() : connectionSlot;
	if (slot == MultiConnection::ConnectionSlotNotFound)
	{
		std::cout << "Refused connection" << std::endl;
		// TODO: CONNECTION REFUSED. NO ROOM
		return ;
	}
	
	if (connectionSlot == MultiConnection::ConnectionSlotNotFound)
	{
		EstablishNewConnection(slot, aAddress);
	}

	NetMessage response(msg.myMessageID);
	response.mySenderID = slot;

	mySocket.Send(response, aAddress);
}

int Network::MultiConnection::GetConnectionSlot(const Address& aAddress) const
{
	for (int i = 0; i < myConnections.size(); i++)
	{
		if (myConnections[i].IsConnected() && myConnections[i].myAddress == aAddress)
		{
			return i;
		}
	}
	return -1;
}

int Network::MultiConnection::FindFreeConnectionSlot() const
{
	for (int i = 0; i < Constants::MAX_CLIENT_COUNT; i++)
	{
		if (!myConnections[i].IsConnected())
		{
			return i;
		}
	}
	return -1;
}
