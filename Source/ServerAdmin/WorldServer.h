#pragma once
#include "Common.h"

struct AreaServerInstance
{
	Network::Address myAddress;
};

class WorldServer
{
public:
	void Startup();

	void InstantiateAreaServers();

private:
	std::vector<AreaServerInstance> myAreaServers;
	Network::UDPSocket mySocket;
	double myTime;
};

