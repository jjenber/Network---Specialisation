#pragma once
#include "Config.h"
#include <string>
#include <limits>
#include <cassert>

#pragma warning ( disable : 26812 )

namespace Network
{
	using MessageID_t  = NET_MSG_TYPE;
	using HeaderSize_t = NET_MSG_HEADER_SIZE_TYPE;
	using ClientSlot_t = NET_CLIENT_SLOT_TYPE;

	enum eNetMessageID : MessageID_t
	{
		eNETMESSAGE_NONE = 0,
		eNETMESSAGE_AREA_SERVER_HANDSHAKE,
		eNETMESSAGE_AREA_SERVER_DEPLOY,

		eNETMESSAGE_CLIENT_HANDSHAKE = 64,
		eNETMESSAGE_SERVER_FULL,
		eNETMESSAGE_ACKNOWLEDGEMENT,
		eNETMESSAGE_HEARTBEAT,
		eNETMESSAGE_CHAT,

		eNETMESSAGE_RELIABLE_ID = 126,
		eNETMESSAGE_DISCONNECT,
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

	template<class T>
	static constexpr HeaderSize_t GetSizeOfMessage()
	{
		static_assert(std::is_base_of_v<NetMessage, T>, "Type must derive from NetMessage.");
		return (sizeof(T) - sizeof(NetMessage)) + GetSizeOfMessage<NetMessage>();
	}
	template <> 
	static constexpr HeaderSize_t GetSizeOfMessage<NetMessage>()
	{
		return sizeof(NetMessage) - sizeof(void*);
	}


	class HandshakeMessage : public NetMessage
	{
	public:
		HandshakeMessage(int aClientSlot = UCHAR_MAX) 
			: NetMessage(eNETMESSAGE_CLIENT_HANDSHAKE) 
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
#pragma region ReliableMessages
	class ReliableNetMessage : public NetMessage
	{
		friend class ReliableNetMessageQueue;
	public:
		ReliableNetMessage(const eNetMessageID aMessageType = eNETMESSAGE_NONE)
			: NetMessage(aMessageType), mySequenceNr(USHRT_MAX)
		{
			mySize = GetSizeOfMessage<ReliableNetMessage>();
			assert(mySize == 6);
		}
		unsigned short mySequenceNr;
	};
	class AcknowledgementMessage : public NetMessage
	{
	public:
		AcknowledgementMessage(unsigned short aSequenceNr = USHRT_MAX) :
			mySequenceNr(aSequenceNr), NetMessage(eNETMESSAGE_ACKNOWLEDGEMENT) 
		{
			mySize = GetSizeOfMessage<AcknowledgementMessage>();
		}
		unsigned short mySequenceNr;
	};

#pragma endregion
#pragma pack(pop)
}