#include "pch.h"
#include <cassert>
#include "NetMessage.h"

Network::ChatMessage::ChatMessage(const std::string& aMessage)
	: NetMessage(eMESSAGE_TYPE_CHAT)
{
	mySize = 
		static_cast<MessageType_t>(sizeof(NetMessage)) + 
		static_cast<MessageType_t>(sizeof(Constants::MAX_CHAT_LENGTH)) + 
		sizeof(myLength);

	SetText(aMessage);
}

void Network::ChatMessage::SetText(const std::string& aChatMessage)
{
	assert(aChatMessage.size() < Constants::MAX_CHAT_LENGTH - 1 && "Chat message is too long. Consider increasing MAX_CHAT_LENGTH in the config.");
	ZeroMemory(myMessage, Constants::MAX_CHAT_LENGTH);
	memcpy(&myMessage, aChatMessage.data(), aChatMessage.length());
}
