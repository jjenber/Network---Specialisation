#pragma once
#include "Config.h"

#pragma warning ( disable : 26812 )

namespace Networking
{
	using msg_type = NET_MSG_TYPE;
	
	enum eMessage : msg_type
	{
		eMESSAGE_TYPE_NONE = 0,
		eMESSAGE_TYPE_HANDSHAKE,
		eMESSAGE_TYPE_CHAT,
	};

#pragma pack(1)
	class NetMessage
	{
		template<size_t> friend class NetMessageQueue;
		
		NetMessage(const eMessage aMessageType)
			: mySenderID(0), myMessageType(aMessageType) {}
		NetMessage() : NetMessage(eMESSAGE_TYPE_NONE) {}
		virtual ~NetMessage() {}

		constexpr eMessage GetMessageType() const { return myMessageType; }

		const eMessage myMessageType;
		unsigned short mySenderID;
	};
}