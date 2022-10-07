#pragma once
#include "datapacket.h"
#include <string>

#define max_sprite_packet_sheet_name_length 64

struct ISpritePacket : IDataPacket
{
	unsigned char index;
	char sheetname[max_sprite_packet_sheet_name_length];
};

struct OSpritePacket : ODataPacket
{
	unsigned char index;
	char sheetname[max_sprite_packet_sheet_name_length];
	OSpritePacket(std::string spriteName, unsigned int index);
};