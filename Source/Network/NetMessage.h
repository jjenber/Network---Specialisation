#pragma once
#include "Config.h"
#include <string>
#include <limits>

#pragma warning ( disable : 26812 )

namespace Network
{
	using MessageID_t  = NET_MSG_TYPE;
	using HeaderSize_t = NET_MSG_HEADER_SIZE_TYPE;
	using ClientSlot_t = NET_CLIENT_SLOT_TYPE;

	enum eNetMessageID : MessageID_t
	{
		eNETMESSAGE_NONE = 0,
		eNETMESSAGE_DISCONNECT,
		eNETMESSAGE_HANDSHAKE,
		eNETMESSAGE_CHAT,

		eNETMESSAGE_RELIABLE_ID = 126,
	};

#pragma pack(push, 1)
	class NetMessage
	{
		template<size_t> friend class NetMessageQueue;
	
	public:
		static constexpr int Size() { return sizeof(NetMessage) - sizeof(void*); }

		NetMessage(const eNetMessageID aMessageType)
			: myMessageID(aMessageType), mySize(NetMessage::Size()) {}

		NetMessage() : NetMessage(eNETMESSAGE_NONE) {}
		virtual ~NetMessage() {}

		constexpr eNetMessageID GetMessageID() const { return myMessageID; }

		eNetMessageID myMessageID;
		ClientSlot_t  mySenderID = (std::numeric_limits<ClientSlot_t>::max());
		HeaderSize_t  mySize;
	};

	class ReliableNetMessage : public NetMessage
	{
		template<size_t> friend class ReliableNetMessageQueue;
	public:
		ReliableNetMessage(const eNetMessageID aMessageType = eNETMESSAGE_NONE) 
			: NetMessage(aMessageType), mySequenceNr(USHRT_MAX)
		{ 
			mySize = static_cast<HeaderSize_t>(sizeof(ReliableNetMessage) - sizeof(void*)); 
		}
		unsigned short GetSequence() const { return mySequenceNr; }
	private:
		unsigned short mySequenceNr;
	};

	class HandshakeMessage : public NetMessage
	{
	public:
		HandshakeMessage(int aClientSlot = UCHAR_MAX) 
			: NetMessage(eNETMESSAGE_HANDSHAKE) 
		{
			mySize += sizeof(HandshakeMessage) - sizeof(NetMessage);
		}

		unsigned char myClientSlot = UCHAR_MAX;
	};

	class ChatMessage : public NetMessage
	{
	public:
		ChatMessage(const std::string& aChatMessage = std::string());
		void SetText(const std::string& aChatMessage);
	
	private:
		char myMessage[Constants::MAX_CHAT_LENGTH];
		unsigned short myLength = 0;
	};
#pragma pack(pop)
}