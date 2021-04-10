#pragma once
#define NOMINMAX

#include <functional>

#pragma comment(lib, "ws2_32.lib")

#define NET_MSG_TYPE unsigned char
#define NET_CLIENT_SLOT_TYPE unsigned char
#define NET_MSG_HEADER_SIZE_TYPE unsigned short
#define IPADDRESS_TYPE unsigned short
#define MAX_CLIENT_COUNT 128

namespace Network
{
	class NetMessage;
	namespace Constants
	{
		constexpr int MAX_CHAT_LENGTH = 512;
		constexpr unsigned short WORLD_TO_AREA_PORT   = 53255u;
		constexpr unsigned short WORLD_TO_CLIENT_PORT = 53256u;
		constexpr unsigned int MAX_BUFFER_SIZE = 512;
		constexpr unsigned int MAX_MESSAGE_PAYLOAD_SIZE = 496;

		constexpr int MAX_RESEND_ATTEMPTS = 10;
		constexpr int RESEND_WAIT_TIME_MS = 100;

		constexpr float CLIENT_TIME_OUT_S = 5.f;
	}
}
