#pragma once
#include "Config.h"
#include <string>

#pragma warning ( disable : 26812 )

namespace Network
{
	using MessageID_t = NET_MSG_TYPE;
	using HeaderSize_t = NET_MSG_HEADER_SIZE_TYPE;

	enum eNetMessageID : MessageID_t
	{
		eNETMESSAGE_NONE = 0,
		eNETMESSAGE_CHAT,

		eNETMESSAGE_RELIABLE_ID = 126,
		eNETMESSAGE_ACKNOWLEDGEMENT,
		eNETMESSAGE_HANDSHAKE
	};

#pragma pack(1)
	class NetMessage
	{
		template<size_t> friend class NetMessageQueue;
	
	public:
		NetMessage(const eNetMessageID aMessageType)
			: myConnectionID(0), myMessageType(aMessageType), myBodySize(0) {}

		NetMessage() : NetMessage(eNETMESSAGE_NONE) {}
		virtual ~NetMessage() {}

		constexpr eNetMessageID GetMessageType() const { return myMessageType; }
	
	protected:
		eNetMessageID myMessageType;
		unsigned short myConnectionID;
		HeaderSize_t   myBodySize;
	};

	class ReliableNetMessage : public NetMessage
	{
		template<size_t> friend class ReliableNetMessageQueue;

	public:
		using NetMessage::NetMessage;
		ReliableNetMessage() : myAckID(USHRT_MAX) { myBodySize = static_cast<HeaderSize_t>(sizeof(ReliableNetMessage)) - static_cast<HeaderSize_t>(sizeof(NetMessage)); }
	private:
		unsigned short myAckID;
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