#pragma once
#include <string>

namespace Network
{
	class Address
	{
	public:
		Address() = default;
		Address(const std::string& aAddress, unsigned short aPort);
		bool operator==(const Address& aAddress) const;

		void SetFromSockAddr(const sockaddr_in& aSockAddr);
		void SetIPFromString(const std::string& aString);
		
		void ToSockAddr(sockaddr_in& aSockAddr) const;

		std::string ToString() const;
		
		uint32_t GetIP() const { return myIP; }
		uint16_t GetPort() const { return myPort; }
	private:
		union
		{
			uint32_t myIP;
			uint8_t myIPBytes[4];
		};
		u_short myPort;
	};

}
namespace std
{
	template <>
	struct hash<Network::Address>
	{
		std::size_t operator()(const Network::Address& address) const
		{
			return 
				( std::hash<uint32_t>()(address.GetIP())
				^ (std::hash<u_short>()(address.GetPort()) << 1));
		}
	};
}

