#include "pch.h"
#include "UnaryConnection.h"
#include "Timer\Timer.h"
#include <iostream>

void Network::UnaryConnection::ConnectAsync(const Address& aAddress, std::function<void()> myCallback)
{
    
}

void Network::UnaryConnection::OnReceivedMessage(char aBuffer[Constants::MAX_BUFFER_SIZE], const Network::Address& aFromAddress)
{

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
			myMessageQueue.EnqueueReceivedBuffer(buf);
			if (myMessageQueue.Peek() == aHandshakeID)
			{
				myMessageQueue.Dequeue(msg);
				mySlot = msg.mySenderID;
				myAddress = mySocket.GetBoundAddress();
				myConnectionStatus = eConnectionStatus::Connected;
			}
		}

		if (timeout < 0.f)
		{
			myConnectionStatus = eConnectionStatus::TimedOut;
		}

		myMessageQueue.Clear();
	}

	return myConnectionStatus == eConnectionStatus::Connected;
}
