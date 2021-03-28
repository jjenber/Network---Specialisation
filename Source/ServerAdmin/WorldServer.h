#pragma once
#include "NetMessage\NetMessageQueue.hpp"
#include "Connection\MultiConnection.h"
#include "NetMessage\ReliableNetMessageQueue.h"
#include <bitset>

constexpr auto MAX_AREA_SERVERS = 8;

struct AreaServerInstance
{
	Network::Address myAddress;
};

class WorldServer
{
public:
	void Startup();

	void InstantiateAreaServers();

	void Update(const float aDeltatime);

private:
	void OnAreaServerMessageReceived(Network::eMessageStatus aStatus, Network::MessageID_t aID, size_t aSize, void* aData);

	std::array<AreaServerInstance, MAX_AREA_SERVERS> myInstances;
	Network::MultiConnection myConnection;

	Network::NetMessageQueue<1024> myAreaServerMessages;
	double myTime;
};

