#pragma once
#include "datapacket.h"

struct IFlashPacket : ODataPacket
{
	unsigned char r, g, b, power, frames;
};

struct OFlashPacket : ODataPacket
{
	unsigned char r, g, b, power, frames;
	OFlashPacket(unsigned char r, unsigned char g, unsigned char b, unsigned char power, unsigned char frames) 
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->power = power;
		this->frames = frames;

		this->slot = DataPacketSlot::pflash;
		this->flags = DataPacketFlag::broadcastForMap;
	}
};
