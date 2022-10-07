#pragma once
#include "basepacket.h"

struct ConnectPacket : IncomingPacket
{
	unsigned int id;
};
