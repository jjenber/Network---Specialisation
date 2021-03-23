#include "AreaServer.h"
#include "NetMessage\NetMessage.h"
#include "Timer\Timer.h"
#include <iostream>

void AreaServer::Startup()
{
	myWorldServerAddress = Network::Address("127.0.0.1", Network::Constants::DEFAULT_PORT);
	myWorldServerSocket.SetBlocking(false);
	myIsRunning = true;
}

Network::eConnectionStatus AreaServer::ConnectToWorldServer()
{
	using namespace Network;
	Timer timer;
	float timeout = 20.f;
	NetMessage msg(eNETMESSAGE_AREA_SERVER_HANDSHAKE);
	myConnectionStatus = eConnectionStatus::Connecting;
	char buf[Constants::MAX_BUFFER_SIZE];

	while (myConnectionStatus == eConnectionStatus::Connecting)
	{
		timeout -= timer.Update();
		if (timer.GetTotalTime() > 0.5f)
		{
			myWorldServerSocket.Send(msg, myWorldServerAddress);
			std::cout << "Sending handshake from: " << myWorldServerSocket.GetBoundAddress().ToString() << " to " << myWorldServerAddress.ToString() << std::endl;
			timer.Reset();
		}

		ZeroMemory(buf, Constants::MAX_BUFFER_SIZE);
		Address from;
		if (myWorldServerSocket.Receive(buf, from))
		{
			myMessageQueue.EnqueueReceivedBuffer(buf);
			switch (myMessageQueue.Peek())
			{
			case eNETMESSAGE_AREA_SERVER_HANDSHAKE:
			{
				NetMessage msg;
				myMessageQueue.Dequeue(msg);
				myServerID = msg.mySenderID;
				myConnectionStatus = eConnectionStatus::Connected;
				break;
			}
			default:
				break;
			}
		}

		if (timeout < 0.f)
		{
			myConnectionStatus = eConnectionStatus::TimedOut;
		}
	}

	return myConnectionStatus;
}

bool AreaServer::Update()
{
	using namespace Network;
	Address from;
	char buf[Constants::MAX_BUFFER_SIZE];
	while (myWorldServerSocket.Receive(buf, from))
	{
		myMessageQueue.EnqueueReceivedBuffer(buf);
	}
}
