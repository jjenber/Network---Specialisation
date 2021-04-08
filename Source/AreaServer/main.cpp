#include <iostream>
#include "Context\Context.h"
#include "Timer\Timer.h"
#include "AreaServer.h"

int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
		std::cout << argv[i] << std::endl;

	std::cout << "AreaServer: Connecting to world server..." << std::endl;

	Network::Context context;
	AreaServer areaServer;
	if (!areaServer.Startup()) return 0;


	Timer timer;
	while (areaServer.Update(timer.GetDeltaTime()))
	{
		timer.Update();
	}
}
