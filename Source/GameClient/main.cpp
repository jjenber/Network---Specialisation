
#include <iostream>
#include "Context\Context.h"
#include "Client.h"

int main()
{
	Network::Context context;
	Network::Client client;
	client.Init();

	std::cout << "Connecting" << std::endl;
	client.ConnectToServer();
	std::cout << "Connected!!" << std::endl;

	while (true)
	{
		client.Update();
	}
}
