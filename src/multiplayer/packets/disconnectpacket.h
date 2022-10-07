#pragma once
#include "basepacket.h"

struct DisconnectPacket : IncomingPacket 
{
	unsigned int id;
};