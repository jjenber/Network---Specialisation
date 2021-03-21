
#include <iostream>
#include "Context\Context.h"
#include "Timer\Timer.h"
#include "Client.h"

int main()
{
	Network::Context context;
	Network::Client client;
	client.Init();

	std::cout << "Connecting" << std::endl;
	client.ConnectToServer();
	std::cout << "Connected at slot: " << client.GetClientSlot() << std::endl;

	Timer timer;
	float time = 0;
	while (client.GetConnectionStatus() != eConnectionStatus::Disconnected)
	{
		timer.Update();
		time += timer.GetDeltaTime();
		
		client.Update();
		if (time > 3.f)
		{
			time = 0.f;
			client.Disconnect();
		}
	}
}
