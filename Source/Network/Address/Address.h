#pragma once
#include <string>

struct sockaddr_in;

namespace Network
{
	
	using ipaddress_t = IPADDRESS_TYPE;
	class Address
	{
	public:
		enum class eType : ipaddress_t
		{
			Undefined,
			Ipv4,
		};

		Address();
		Address(const std::string& aAddress, unsigned short aPort);
		
		bool operator==(const Address& aAddress) const;

		void SetFromSockAddr(const sockaddr_in& aSockAddr);
		void SetIPFromString(const std::string& aString);

		void Clear();

		void ToSockAddr(sockaddr_in& aSockAddr) const;
		std::string ToString() const;

		bool Valid() const;

		uint32_t GetIP() const { return myIP; }
		uint16_t GetPort() const { return myPort; }
	private:
		union
		{
			uint32_t myIP;
			uint8_t myIPBytes[4];
		};
		unsigned short myPort;
		eType   myType;
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
				^ (std::hash<unsigned short>()(address.GetPort()) << 1));
		}
	};
}

