#pragma once
#include "datapacket.h"

struct IMoveSpeedPacket : IDataPacket
{
	unsigned char speed;
};

struct OMoveSpeedPacket : ODataPacket
{
	unsigned char speed;
	OMoveSpeedPacket(unsigned char speed);
};
