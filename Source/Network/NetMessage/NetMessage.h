#pragma once
#include "Config.h"
#include <string>
#include <limits>
#include <cassert>
#include <iostream>

#pragma warning ( disable : 26812 )

namespace Network
{
	enum class eMessageStatus
	{
		Failed,
		Success,
		TimedOut,
	};

	using MessageID_t  = NET_MSG_TYPE;
	using HeaderSize_t = NET_MSG_HEADER_SIZE_TYPE;
	typedef NET_CLIENT_SLOT_TYPE ClientSlot_t;

	enum eNetMessageID : MessageID_t
	{
		eNETMESSAGE_NONE = 0,
		eNETMESSAGE_AS_HANDSHAKE,

		eNETMESSAGE_CLIENT_HANDSHAKE = 64,
		eNETMESSAGE_SERVER_FULL,
		eNETMESSAGE_ACKNOWLEDGEMENT,
		eNETMESSAGE_HEARTBEAT,
		eNETMESSAGE_CHAT,

		eNETMESSAGE_RELIABLE_ID = 126,
		eNETMESSAGE_R_HANDSHAKE_RESPONSE,
		eNETMESSAGE_R_AS_DEPLOY,
		eNETMESSAGE_R_AS_STATUS,
		eNETMESSAGE_R_AS_REQUEST_IDS,
		eNETMESSAGE_R_AS_REQUEST_IDS_RESPONSE,
		eNETMESSAGE_R_AS_REQUEST_ENTITY_STATES,
		eNETMESSAGE_R_DISCONNECT,
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

	class AcknowledgementMessage : public NetMessage
	{
	public:
		AcknowledgementMessage(MessageID_t aAcknowledgedMessageID = eNETMESSAGE_NONE, unsigned short aSequenceNr = USHRT_MAX) :
			myAcknowledgedMessageID(aAcknowledgedMessageID), mySequenceNr(aSequenceNr), NetMessage(eNETMESSAGE_ACKNOWLEDGEMENT)
		{
			mySize = GetSizeOfMessage<AcknowledgementMessage>();
		}
		MessageID_t myAcknowledgedMessageID;
		unsigned short mySequenceNr;
	};

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
		virtual ~ReliableNetMessage() {};
		unsigned short mySequenceNr;
	};

	class DeployAreaServer : public ReliableNetMessage
	{
		friend class ReliableNetMessageQueue;
	public:
		DeployAreaServer(unsigned char aRegionID = UCHAR_MAX)
			: ReliableNetMessage(eNETMESSAGE_R_AS_DEPLOY), myRegionID(aRegionID) {
			mySize = GetSizeOfMessage<DeployAreaServer>();
		};
		unsigned char myRegionID;
	};

	class RequestUniqueIDs : public ReliableNetMessage
	{
		friend class ReliableNetMessageQueue;
	public:
		static constexpr size_t MaxCount = 50;

		RequestUniqueIDs(unsigned char aCount = 0)
			: ReliableNetMessage(eNETMESSAGE_R_AS_REQUEST_IDS), myCount(aCount) {
			SetCount(aCount);
		};
		void SetCount(unsigned char aCount)
		{
			myCount = aCount;
			mySize = GetSizeOfMessage<RequestUniqueIDs>() - ((size_t(MaxCount) - aCount) * sizeof(myLocalIDs[0]));
		};

		unsigned char myCount;
		uint32_t myLocalIDs[MaxCount]{};
	};

	class ResponseUniqueIDs : public ReliableNetMessage
	{
		friend class ReliableNetMessageQueue;
	public:
		ResponseUniqueIDs(unsigned char aCount)
			: ReliableNetMessage(eNETMESSAGE_R_AS_REQUEST_IDS_RESPONSE), myCount(aCount) {
			mySize = GetSizeOfMessage<ResponseUniqueIDs>() - ((size_t(100) - (aCount * 2)) * sizeof(myMappedIDs[0]));
		};

		unsigned char myCount;
		/// Even: local, Odd: unique
		uint32_t myMappedIDs[100]{};
	};

	class AreaServerStatus : public ReliableNetMessage
	{
		friend class ReliableNetMessageQueue;
	public:
		AreaServerStatus(unsigned char aStatus = UCHAR_MAX) : ReliableNetMessage(eNETMESSAGE_R_AS_STATUS), myStatus(aStatus) {
			mySize = GetSizeOfMessage<AreaServerStatus>();
		};
		unsigned char myStatus;
	};
#pragma pack(pop)
}
