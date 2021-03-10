#include "pch.h"
#include "Server.h"

void Network::Server::Startup()
{
	myUDPSocket.SetBlocking(false);
	myUDPSocket.BindToLocal();
}

void Network::Server::Update()
{

}
