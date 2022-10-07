#include "spritepacket.h"
#include <memory.h>

OSpritePacket::OSpritePacket(std::string spriteName, unsigned int index) 
{
	this->flags = DataPacketFlag::broadcastForMap | DataPacketFlag::saveForPlayer;
	this->index = (char)index;
	memset(this->sheetname, 0, max_sprite_packet_sheet_name_length);
	memcpy(this->sheetname, spriteName.c_str(), std::min((int)spriteName.length(), max_sprite_packet_sheet_name_length));
	this->sheetname[max_sprite_packet_sheet_name_length - 1] = 0;
	this->slot = DataPacketSlot::sprite;
} 