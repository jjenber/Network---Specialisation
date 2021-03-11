#pragma once
#include "Config.h"
#include <string>

#pragma warning ( disable : 26812 )

namespace Network
{
	using MessageType_t = NET_MSG_TYPE;
	using HeaderSize_t = NET_MSG_HEADER_SIZE_TYPE;

	enum eMessage : MessageType_t
	{
		eMESSAGE_TYPE_NONE = 0,
		eMESSAGE_TYPE_CHAT,
		eMESSAGE_TYPE_NON_GUARANTEED_COUNT,

		eMESSAGE_TYPE_HANDSHAKE_G,
	};

#pragma pack(1)
	class NetMessage
	{
		template<size_t> friend class NetMessageQueue;
	
	public:
		NetMessage(const eMessage aMessageType)
			: myConnectionID(0), myMessageType(aMessageType), mySize(0) {}

		NetMessage() : NetMessage(eMESSAGE_TYPE_NONE) {}
		virtual ~NetMessage() {}

		constexpr eMessage GetMessageType() const { return myMessageType; }
	
	protected:
		const eMessage myMessageType;
		unsigned short myConnectionID;
		HeaderSize_t   mySize;
	};

	class NetMessageGuaranteed : public NetMessage
	{
	public:

	};

	class ChatMessage : public NetMessage
	{
	public:
		ChatMessage(const std::string& aChatMessage = std::string());

		void SetText(const std::string& aChatMessage);
	
	private:
		unsigned short myLength = 0;
		char myMessage[Constants::MAX_CHAT_LENGTH];
	};
}