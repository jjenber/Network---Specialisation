
#include <iostream>
#include "Context\Context.h"
#include "Timer\Timer.h"
#include "Client.h"

int main()
{
	Network::Context context;
	Network::Client client;

	std::cout << "Connecting to World Server " << std::endl;
	
	client.Init();
	
	if (client.GetConnectionStatus() != Network::eConnectionStatus::Connected)
	{
		std::cout << "Failed to connect: " << (int)client.GetConnectionStatus() << std::endl;
		return 0;
	}
	
	std::cout << "Connected at slot: " << client.GetClientSlot() << std::endl;

	Timer timer;
	float time = 0;

	while (client.GetConnectionStatus() != Network::eConnectionStatus::Disconnected)
	{
		client.Update(timer.Update());
	}

	return 0;
}
