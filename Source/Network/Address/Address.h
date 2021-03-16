#pragma once
#include <string>
namespace Network
{
	class Address
	{
	public:
		Address() = default;
		Address(const std::string& aAddress, unsigned short aPort);
		Address(unsigned short aPort);
	private:
		sockaddr_in mySockAddr = { 0 };
	};
}

