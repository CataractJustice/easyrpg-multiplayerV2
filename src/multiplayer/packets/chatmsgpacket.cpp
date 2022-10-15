#include "chatmsgpacket.h"
#include "memory.h"

OChatMsgPacket::OChatMsgPacket(const std::string& message, unsigned char flags) 
{
	memcpy(this->message, message.c_str(), std::min((int)message.length(), max_chat_message_length));
	this->message[max_chat_message_length - 1] = 0;
	this->flags = flags;
	this->slot = DataPacketSlot::chatmsg;
}