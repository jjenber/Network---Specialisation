#pragma once
#include "Config.h"
#include "Address\Address.h"
#include <CommonUtilities\Math\Vector3.h>
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
		eNETMESSAGE_SERVER_FULL,
		eNETMESSAGE_ACKNOWLEDGEMENT,
		eNETMESSAGE_CHAT,

		// Client
		eNETMESSAGE_CLIENT_HANDSHAKE,
		eNETMESSAGE_CLIENT_MOVE,

		// Area Server Messages
		eNETMESSAGE_AS_HANDSHAKE = 64,
		eNETMESSAGE_AS_REQUEST_ENTITY_STATES,
		eNETMESSAGE_AS_RESPONSE_ENTITY_STATES,
		eNETMESSAGE_AS_CLIENT_STATES,

		eNETMESSAGE_RELIABLE_ID = 126,
		eNETMESSAGE_R_HANDSHAKE_RESPONSE,
		eNETMESSAGE_R_DISCONNECT,
		
		// Reliable Area Server Messages 
		eNETMESSAGE_R_AS_DEPLOY,
		eNETMESSAGE_R_AS_INITIALIZED,
		eNETMESSAGE_R_AS_STATUS,
		eNETMESSAGE_R_AS_REQUEST_IDS,
		eNETMESSAGE_R_AS_REQUEST_IDS_RESPONSE,
		eNETMESSAGE_R_AS_CLIENT_MIGRATE,

		// Reliable Client Messages
		eNETMESSAGE_R_CLIENT_ABORT_MIGRATE,
		eNETMESSAGE_R_CLIENT_ENTER_AREA,
		eNETMESSAGE_R_CLIENT_EXIT_AREA,
		eNETMESSAGE_R_CLIENT_VALIDATE_TOKEN,
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

		eNetMessageID GetMessageID() const { return myMessageID; }

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

	class ClientMoveMessage : public NetMessage
	{
	public:
		ClientMoveMessage(uint32_t aUniqueID = UINT_MAX, const CommonUtilities::Vector3f& aVelocity = CommonUtilities::Vector3f()) 
			: NetMessage(eNETMESSAGE_CLIENT_MOVE), myUniqueID(aUniqueID), myVelocity(aVelocity)
		{
			mySize = GetSizeOfMessage<ClientMoveMessage>();
		};
		CommonUtilities::Vector3f myVelocity;
		uint32_t myUniqueID;
	};

	class ClientServerPosition : public NetMessage
	{
	public:
		ClientServerPosition(const CommonUtilities::Vector3f& aPosition = CommonUtilities::Vector3f()) 
			: NetMessage(eNETMESSAGE_CLIENT_MOVE), myPosition(aPosition)
		{
			mySize = GetSizeOfMessage<ClientServerPosition>();
		};
		CommonUtilities::Vector3f myPosition;
	};

	struct EntityState
	{
		EntityState() = default;
		EntityState(float aX, float aZ, uint32_t aUniqueID) : myX(aX), myZ(aZ), myUniqueID(aUniqueID) {}
		float myX;
		float myZ;
		uint32_t myUniqueID;
	};

	class EntityStatesMessage : public NetMessage
	{
	public:
		static constexpr size_t MaxCount = Constants::MAX_MESSAGE_PAYLOAD_SIZE / sizeof(EntityState);

		EntityStatesMessage(uint8_t aCount = 0) : NetMessage(eNETMESSAGE_AS_RESPONSE_ENTITY_STATES), myCount(aCount)
		{
			SetCount(aCount);
		};

		void SetCount(uint8_t aCount)
		{ 
			myCount = aCount;
			mySize = GetSizeOfMessage<EntityStatesMessage>() - ((size_t(MaxCount) - aCount) * sizeof(EntityState));
		};

		uint8_t myCount;
		EntityState myData[MaxCount];
	};

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
		AcknowledgementMessage(MessageID_t aAcknowledgedMessageID = eNETMESSAGE_NONE, unsigned short aSequenceNr = USHRT_MAX) 
			: myAcknowledgedMessageID(aAcknowledgedMessageID), mySequenceNr(aSequenceNr), NetMessage(eNETMESSAGE_ACKNOWLEDGEMENT)
		{
			mySize = GetSizeOfMessage<AcknowledgementMessage>();
		}
		MessageID_t myAcknowledgedMessageID;
		unsigned short mySequenceNr;
	};

#pragma region Reliable Messages

	class ReliableNetMessage : public NetMessage
	{
	public:
		ReliableNetMessage(const eNetMessageID aMessageType = eNETMESSAGE_NONE)
			: NetMessage(aMessageType), mySequenceNr(USHRT_MAX)
		{
			mySize = GetSizeOfMessage<ReliableNetMessage>();
		}

		virtual ~ReliableNetMessage() {};
		unsigned short mySequenceNr;
	};

	class DeployAreaServer : public ReliableNetMessage
	{
		friend class ReliableNetMessageQueue;
	public:
		DeployAreaServer(uint8_t aRegionID = UCHAR_MAX)
			: ReliableNetMessage(eNETMESSAGE_R_AS_DEPLOY), myRegionID(aRegionID) {
			mySize = GetSizeOfMessage<DeployAreaServer>();
		};
		uint8_t myRegionID;
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
		void SetCount(uint8_t aCount)
		{
			myCount = aCount;
			mySize = GetSizeOfMessage<RequestUniqueIDs>() - ((size_t(MaxCount) - aCount) * sizeof(myLocalIDs[0]));
		};

		uint8_t myCount;
		uint32_t myLocalIDs[MaxCount]{};
	};

	class ResponseUniqueIDs : public ReliableNetMessage
	{
	public:
		ResponseUniqueIDs(uint8_t aCount = 0)
			: ReliableNetMessage(eNETMESSAGE_R_AS_REQUEST_IDS_RESPONSE), myCount(aCount) {
			mySize = GetSizeOfMessage<ResponseUniqueIDs>() - ((size_t(100) - (size_t(aCount) * 2)) * sizeof(myMappedIDs[0]));
		};

		uint8_t myCount;

		/// Even: local, Odd: unique
		uint32_t myMappedIDs[100]{};
	};

	class AreaServerStatus : public ReliableNetMessage
	{
	public:
		AreaServerStatus(uint8_t aStatus = UCHAR_MAX) : ReliableNetMessage(eNETMESSAGE_R_AS_STATUS), myStatus(aStatus) 
		{
			mySize = GetSizeOfMessage<AreaServerStatus>();
		};
		unsigned char myStatus;
	};

	class AreaServerInitialized : public ReliableNetMessage
	{
	public:
		AreaServerInitialized(Address aClientConnectionAddress = Address()) 
			: ReliableNetMessage(eNETMESSAGE_R_AS_INITIALIZED), myClientConnectionAddress(aClientConnectionAddress)
		{
			mySize = GetSizeOfMessage<AreaServerInitialized>();
		};
		Address myClientConnectionAddress;
	};

	class ClientEnterAreaMessage : public ReliableNetMessage
	{
	public:
		ClientEnterAreaMessage(const CommonUtilities::Vector3<uint16_t>& aPosition = CommonUtilities::Vector3<uint16_t>(), uint32_t aAreaServerIP = 0, uint32_t aAreaServerPort = 0, uint32_t aUniqueID = 0, uint8_t aRegion = 0, const Address& aClientAddress = Address(), uint32_t aToken = 0) 
			: ReliableNetMessage(eNETMESSAGE_R_CLIENT_ENTER_AREA),
			myPosition(aPosition),
			myAreaServerIP(aAreaServerIP),
			myAreaServerPort(aAreaServerPort),
			myRegion(aRegion),
			myUniqueID(aUniqueID),
			myToken(aToken),
			myClientAddress(aClientAddress)
		{
			mySize = GetSizeOfMessage<ClientEnterAreaMessage>();
		}

		CommonUtilities::Vector3<uint16_t> myPosition;
		Address  myClientAddress;
		uint32_t myUniqueID;
		uint32_t myAreaServerIP;
		uint32_t myAreaServerPort;
		uint32_t myToken;
		uint8_t  myRegion = UCHAR_MAX;
	};

	class ClientExitAreaMessage : public ReliableNetMessage
	{
	public:
		ClientExitAreaMessage(uint32_t aUniqueID = 0) 
			: ReliableNetMessage(eNETMESSAGE_R_CLIENT_EXIT_AREA),
			  myUniqueID(aUniqueID)
		{
			mySize = GetSizeOfMessage<ClientExitAreaMessage>();
		}
		uint32_t myUniqueID;
	};

	class ClientValidateTokenMessage : public ReliableNetMessage
	{
	public:
		ClientValidateTokenMessage(uint32_t aUniqueID = 0, uint32_t aToken = 0)
			: ReliableNetMessage(eNETMESSAGE_R_CLIENT_VALIDATE_TOKEN),
			  myUniqueID(aUniqueID),
			  myToken(aToken)
		{
			mySize = GetSizeOfMessage<ClientValidateTokenMessage>();
		}
		uint32_t myUniqueID;
		uint32_t myToken;
	};

	class ClientMigrateMessage : public ReliableNetMessage
	{
	public:
		ClientMigrateMessage(uint8_t aX = UINT8_MAX, uint8_t aY = UINT8_MAX)
			: ReliableNetMessage(eNETMESSAGE_R_AS_CLIENT_MIGRATE),
			  myX(aX), 
			  myY(aY) 
		{
			mySize = GetSizeOfMessage<ClientMigrateMessage>();
		};

		uint8_t myX;
		uint8_t myY;
	};

#pragma endregion ReliableMessages

#pragma pack(pop)
}
