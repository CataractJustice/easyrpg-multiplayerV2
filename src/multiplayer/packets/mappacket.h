#pragma once
#include "basepacket.h"

struct MapPacket : OutgoingPacket
{
	short room;
	MapPacket(int room)
	{
		this->type = 0;
		this->room = room;
	}
};
