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
		static constexpr int Size() { return sizeof(NetMessage) - sizeof(void*); }

		NetMessage(const eNetMessageID aMessageType)
			: myConnectionID(0), myMessageID(aMessageType), mySize(NetMessage::Size()) {}

		NetMessage() : NetMessage(eNETMESSAGE_NONE) {}
		virtual ~NetMessage() {}

		constexpr eNetMessageID GetMessageID() const { return myMessageID; }
	
	protected:
		eNetMessageID myMessageID;
		unsigned short myConnectionID;
		HeaderSize_t   mySize;
	};

	class ReliableNetMessage : public NetMessage
	{
		template<size_t> friend class ReliableNetMessageQueue;

	public:
		ReliableNetMessage(const eNetMessageID aMessageType = eNETMESSAGE_NONE) : NetMessage(aMessageType), myAckID(USHRT_MAX)
		{ 
			mySize = static_cast<HeaderSize_t>(sizeof(ReliableNetMessage) - sizeof(void*)); 
		}
	
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