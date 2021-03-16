
#include <iostream>
#include "Context\Context.h"
#include "Client.h"

int main()
{
	Network::Context context;
	Network::Client client;
	client.Init();
	client.ConnectToServer();
	
	while (true)
	{
		client.Update();
	}
}
