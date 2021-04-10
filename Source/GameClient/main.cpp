
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
	std::cout << "Connected at World Server slot " << (int)client.GetWorldServerClientSlot() << std::endl;

	Timer timer;
	const float frameTime = 0.017f;
	float time = frameTime;
	int f = 0;
	while (client.GetConnectionStatus() != Network::eConnectionStatus::Disconnected)
	{
		time += timer.Update();

		while (time > frameTime)
		{
			time -= frameTime;
			client.Update(frameTime);
		}
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(10ms); // Simulate game/rendering time
	}

	return 0;
}
