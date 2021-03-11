#include "pch.h"
#include "Server.h"

void Network::Server::Startup()
{
	myUDPSocket.SetBlocking(false);
	myUDPSocket.BindToLocal();
}

void Network::Server::Update()
{
	char recvBuf[512];
	Address sender;
	int recievedLength = 0;
	while (myUDPSocket.Receive(recvBuf, 1024, sender, recievedLength))
	{

	}
}
