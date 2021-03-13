#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Config.h"

namespace Network
{
	class Context
	{
	public:
		Context();
		~Context();
		Context(const Context&) = delete;
		Context(Context&&) = delete;
		Context& operator=(const Context&) = delete;
		Context& operator=(Context&&) = delete;
	};
}

