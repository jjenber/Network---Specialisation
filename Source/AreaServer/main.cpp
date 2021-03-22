#include <iostream>
#include "Context\Context.h"
#include "AreaServer.h"

int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
		std::cout << argv[i] << std::endl;

	Network::Context context;

	AreaServer areaServer;
	areaServer.Startup();
	std::cout << "AreaServer: Connecting to world server..." << std::endl;

	if (areaServer.ConnectToWorldServer() == Network::eConnectionStatus::Connected)
	{
		std::cout << "Successfully connected!" << std::endl;
	}
	else
	{
		std::cout << "Connection timed out" << std::endl;
	}
}
