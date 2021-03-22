#include "AreaServer.h"
#include "NetMessage\NetMessage.h"
#include "Timer\Timer.h"
#include <iostream>

void AreaServer::Startup()
{
	myWorldServerSocket.SetBlocking(false);
}

Network::eConnectionStatus AreaServer::ConnectToWorldServer()
{
	using namespace Network;
	Timer timer;
	float timeout = 2.f;
	NetMessage msg(eNETMESSAGE_AREA_SERVER_HANDSHAKE);
	myConnectionStatus = eConnectionStatus::Connecting;
	char buf[Constants::MAX_BUFFER_SIZE];

	while (myConnectionStatus == eConnectionStatus::Connecting)
	{
		timeout -= timer.Update();
		if (timer.GetTotalTime() > 0.5f)
		{
			myWorldServerSocket.Send(msg, myWorldServerAddress);
			timer.Reset();
		}

		ZeroMemory(buf, Constants::MAX_BUFFER_SIZE);
		if (myWorldServerSocket.Receive(buf, myWorldServerAddress))
		{
			myMessageQueue.EnqueueReceived(buf);
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
