#pragma once
#include "datapacket.h"

struct IFacingPacket : IDataPacket
{
	unsigned char facing;
};

struct OFacingPacket : ODataPacket
{
	unsigned char facing;
	OFacingPacket(unsigned char facing);
};
