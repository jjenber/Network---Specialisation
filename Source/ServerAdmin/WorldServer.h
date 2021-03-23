#pragma once
#include "Common.h"
#include "NetMessage\NetMessageQueue.hpp"
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

	void ReceiveIncomingMessages();


private:
	void HandleHandshake(Network::eNetMessageID aID, const Network::Address& aAddress);

	int FindFreeAreaServerSlot() const;
	int FindConnectedAreaServer(const Network::Address& aAddress) const;

	struct AreaServers
	{
		std::array<AreaServerInstance, MAX_AREA_SERVERS> myInstances;
		std::array<Network::Address, MAX_AREA_SERVERS> myAddresses;
		std::bitset<MAX_AREA_SERVERS> myConnected;
	} myAreaServers;

	Network::NetMessageQueue<2048> myIncomingNetMessages;
	Network::UDPSocket mySocket;
	double myTime;
};

