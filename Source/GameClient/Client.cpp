#include "pch.h"
#include "Client.h"
#include "NetMessage\NetMessage.h"
#include "Timer\Timer.h"
#include <iostream>
#include <chrono>

void Network::Client::Init()
{
	myUDPSocket.SetBlocking(false);
	myMainServerAddress = Address("127.0.0.1", Constants::DEFAULT_PORT);
}

void Network::Client::Disconnect()
{
	ReliableNetMessage disconnect(eNETMESSAGE_DISCONNECT);
	disconnect.mySenderID = myClientSlot;
	
	//myReliableMessageQueue.Enqueue(disconnect, myMainServerAddress, 5, 50);
	myConnectionStatus = eConnectionStatus::Disconnecting;
	myUDPSocket.Send(disconnect, myMainServerAddress);
}

void Network::Client::Update()
{
	ReceiveIncomingMessages();
}

void Network::Client::ConnectToServer()
{
	HandshakeMessage msg(eNETMESSAGE_CLIENT_HANDSHAKE);
	
	myConnectionStatus = eConnectionStatus::Connecting;
	myUDPSocket.Send(msg, myMainServerAddress);

	Timer timer;
	while (myConnectionStatus == eConnectionStatus::Connecting)
	{
		timer.Update();
		if (timer.GetTotalTime() > 5.f)
		{
			myUDPSocket.Send(msg, myMainServerAddress);
			timer.Reset();
		}

		ReceiveIncomingMessages();
	}
}

void Network::Client::ReceiveIncomingMessages()
{
	Address addr;
	char recvBuf[Constants::MAX_BUFFER_SIZE]{};

	while (myUDPSocket.Receive(recvBuf, addr))
	{
		myReceivedMessages.EnqueueReceivedBuffer(recvBuf);
	}

	while (!myReceivedMessages.Empty())
	{
		MessageID_t msgID = myReceivedMessages.Peek();
		if (msgID > eNetMessageID::eNETMESSAGE_RELIABLE_ID)
		{
			DecodeReliable(msgID);
		}
		else if (msgID > eNETMESSAGE_NONE)
		{
			Decode(msgID);
		}
	}
}

void Network::Client::Decode(MessageID_t aMessageID)
{
	switch (aMessageID)
	{
	case eNETMESSAGE_CLIENT_HANDSHAKE:
	{
		HandshakeMessage message;
		myReceivedMessages.Dequeue(message);
		myClientSlot = message.myClientSlot;
		myConnectionStatus = eConnectionStatus::Connected;
		break;
	}
	case eNETMESSAGE_HEARTBEAT:
	{
		NetMessage msg;
		myReceivedMessages.Dequeue(msg);
		msg.mySenderID = myClientSlot;
		myUDPSocket.Send(msg, myMainServerAddress);
		break;
	}
	case eNETMESSAGE_ACKNOWLEDGEMENT:
	{
		AcknowledgementMessage msg;
		myReceivedMessages.Dequeue(msg);
		myConnectionStatus = eConnectionStatus::Disconnected;
		break;
	}
	default:
		break;
	}
}

void Network::Client::DecodeReliable(MessageID_t aMessageID)
{
	std::cout << "Recieved reliable: " << aMessageID << std::endl;
	ReliableNetMessage ack;
	myReceivedMessages.Dequeue(ack);
	myUDPSocket.Send(ack, myMainServerAddress);
}
