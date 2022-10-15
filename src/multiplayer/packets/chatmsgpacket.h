#pragma once
#include "datapacket.h"
#include <string>
#define max_chat_message_length 512

struct OChatMsgPacket : ODataPacket
{
	char message[max_chat_message_length];
	OChatMsgPacket(const std::string& message, unsigned char flags);
};


struct IChatMsgPacket : IDataPacket
{
	char message[max_chat_message_length];
};

