#pragma once

namespace Network
{
	enum class eConnectionStatus
	{
		Disconnected,
		Disconnecting,
		Connecting,
		Connected,
		TimedOut,
	};
}
