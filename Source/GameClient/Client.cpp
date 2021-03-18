#include "pch.h"
#include "Client.h"
#include "NetMessage.h"
#include "Timer\Timer.h"
#include <iostream>
#include <chrono>

void Network::Client::Init()
{
	myUDPSocket.SetBlocking(false);
	myMainServerAddress = Address("127.0.0.1", Constants::DEFAULT_PORT);
}

void Network::Client::Update()
{
}

void Network::Client::ConnectToServer()
{
	ReliableNetMessage msg(eNETMESSAGE_HANDSHAKE);
	
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

		RecieveIncomingMessages();
	}
}

void Network::Client::RecieveIncomingMessages()
{
	Address addr;
	char recvBuf[Constants::MAX_BUFFER_SIZE]{};

	while (myUDPSocket.Receive(recvBuf, addr))
	{
		myReceivedMessages.EnqueueReceived(recvBuf);
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
	case eNETMESSAGE_HANDSHAKE:
	{
		HandshakeMessage message;
		myReceivedMessages.Dequeue(message);
		myClientSlot = message.myClientSlot;
		myConnectionStatus = eConnectionStatus::Connected;
		break;
	}
	default:
		break;
	}
}

void Network::Client::DecodeReliable(MessageID_t aMessageID)
{
}
