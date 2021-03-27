#include "pch.h"
#include "MultiConnection.h"

void Network::MultiConnection::Init(size_t aMaxConnected, MessageID_t aHandshakeID, NetMessageCallback_t aCallback)
{
	SetMaxConnections(aMaxConnected);
	SetHandshakeID(aHandshakeID);
	SetReceiveMessageCallback(aCallback);
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

void Network::MultiConnection::Update(const float aDeltatime)
{
	BaseConnection::Update();

	Address fromAddress;
	char recvBuf[Constants::MAX_BUFFER_SIZE]{};

	while (mySocket.Receive(recvBuf, fromAddress))
	{
		myReceivedMessages.EnqueueReceivedBuffer(recvBuf);
		MessageID_t msgID  = myReceivedMessages.Peek();

		int connectionSlot = GetConnectionSlot(fromAddress);
		if (msgID == myHandshakeID)
		{
			HandleHandshakeMessage(connectionSlot, fromAddress);
		}
		else if (msgID == eNETMESSAGE_ACKNOWLEDGEMENT)
		{
			HandleAcknowledgement(connectionSlot);
		}
	}
}

void Network::MultiConnection::DecodeReceived(int aConnectionSlot)
{

}

void Network::MultiConnection::HandleAcknowledgement(int aConnectionSlot)
{
	AcknowledgementMessage ack;
	myReceivedMessages.Dequeue(ack);

	std::shared_ptr<ReliableNetMessage> message = myReliableNetMessageQueue.RemoveMessage(ack.mySequenceNr);
	if (myCallback && message)
	{
		myCallback(eMessageStatus::Success, message->myMessageID, message->mySize, &message->myMessageID);
	}
}

void Network::MultiConnection::EstablishNewConnection(int aConnectionSlot, const Address& aAddress)
{
	myConnections[aConnectionSlot].myAddress = aAddress;
	myConnections[aConnectionSlot].myConnectionStatus = eConnectionStatus::Connected;

	std::cout << "Connection established at slot " << aConnectionSlot << std::endl;
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
