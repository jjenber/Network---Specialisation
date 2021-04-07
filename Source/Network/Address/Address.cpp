#include "pch.h"
#include "Address.h"
#include <iostream>
#include <sstream>

Network::Address::Address() : myIP(0), myPort(0), myType(eType::Undefined)
{
}

Network::Address::Address(const sockaddr_in& aSockAddr)
{
	SetFromSockAddr(aSockAddr);
}

Network::Address::Address(const std::string& aAddress, unsigned short aPort) : myPort(aPort)
{
	SetIPFromString(aAddress);
}

bool Network::Address::operator==(const Address& aAddress) const
{
	return myIP == aAddress.myIP && myPort == aAddress.myPort;
}

void Network::Address::SetFromSockAddr(const sockaddr_in& aSockAddr)
{
	myIP = aSockAddr.sin_addr.s_addr;
	myPort = ntohs(aSockAddr.sin_port);
	if (aSockAddr.sin_family == AF_INET)
	{
		myType = eType::Ipv4;
	}
}

void Network::Address::ToSockAddr(sockaddr_in& aSockAddr) const
{
	aSockAddr.sin_family = AF_INET;
	aSockAddr.sin_port = htons(myPort);
	aSockAddr.sin_addr.s_addr = myIP;
}

void Network::Address::SetIPFromString(const std::string& aString)
{
	// TODO: Sanitize input
	std::istringstream stream(aString);
	char dot;
	int a0, a1, a2, a3;
	stream >> a0 >> dot >> a1 >> dot >> a2 >> dot >> a3;
	myIPBytes[0] = a0;
	myIPBytes[1] = a1;
	myIPBytes[2] = a2;
	myIPBytes[3] = a3;

	myType = Address::eType::Ipv4;
}

void Network::Address::Clear()
{
	myIP = 0;
	myPort = 0;
	myType = eType::Undefined;
}

std::string Network::Address::ToString() const
{
	std::ostringstream stream;
	stream << 
		static_cast<int>(myIPBytes[0]) << '.' << 
		static_cast<int>(myIPBytes[1]) << '.' << 
		static_cast<int>(myIPBytes[2]) << '.' << 
		static_cast<int>(myIPBytes[3]) << ':' <<
		myPort;

	return stream.str();
}

bool Network::Address::Valid() const
{
	return myType != eType::Undefined || myPort == 0;
}


