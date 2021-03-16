#pragma once
#include <functional>

#pragma comment(lib, "ws2_32.lib")

#define NET_MSG_TYPE unsigned char
#define NET_MSG_HEADER_SIZE_TYPE unsigned short

namespace Network
{
	class NetMessage;
	namespace Constants
	{
		constexpr int MAX_CHAT_LENGTH = 512;
		constexpr unsigned short DEFAULT_PORT = 54000u;
		constexpr unsigned int MAX_BUFFER_SIZE = 512;
	
		constexpr size_t MAX_CLIENT_COUNT = 4;

		constexpr int MAX_RESEND_ATTEMPTS = 10;
		constexpr int RESEND_WAIT_TIME_MS = 100;
	}
}
