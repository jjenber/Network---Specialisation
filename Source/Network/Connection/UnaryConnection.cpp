#include "pch.h"
#include "UnaryConnection.h"
#include "Timer\Timer.h"
#include <iostream>

void Network::UnaryConnection::OnReceivedMessage(char aBuffer[Constants::MAX_BUFFER_SIZE], const Network::Address& aFromAddress)
{
	myReceivedMessages.EnqueueReceivedBuffer(aBuffer);
}

bool Network::UnaryConnection::Connect(const Address& aAddress, float aTimeoutInSeconds, eNetMessageID aHandshakeID)
{
	mySocket.SetBlocking(false);
	myConnectedAddress = aAddress;
	myConnectionStatus = eConnectionStatus::Connecting;

	Timer timer;
	float timeout = aTimeoutInSeconds;
	NetMessage msg(aHandshakeID);
	Address from;

	char buf[Constants::MAX_BUFFER_SIZE];
	while (myConnectionStatus == eConnectionStatus::Connecting)
	{
		timeout -= timer.Update();
		if (timer.GetTotalTime() > 0.5f)
		{
			SendOrEnqueue(msg, aAddress);
			timer.Reset();
		}

		memset(buf, 0, Constants::MAX_BUFFER_SIZE);

		while (mySocket.Receive(buf, from))
		{
			myReceivedMessages.EnqueueReceivedBuffer(buf);
			if (myReceivedMessages.Peek() == aHandshakeID)
			{
				myReceivedMessages.Dequeue(msg);
				mySlot = msg.mySenderID;
				myAddress = mySocket.GetBoundAddress();
				myConnectionStatus = eConnectionStatus::Connected;
			}
		}

		if (timeout < 0.f)
		{
			myConnectionStatus = eConnectionStatus::TimedOut;
		}

		myReceivedMessages.Clear();
	}

	return myConnectionStatus == eConnectionStatus::Connected;
}
