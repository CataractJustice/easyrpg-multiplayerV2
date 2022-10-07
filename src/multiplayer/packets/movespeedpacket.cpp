#include "movespeedpacket.h"

OMoveSpeedPacket::OMoveSpeedPacket(unsigned char speed) 
{
	this->flags = DataPacketFlag::broadcastForMap | DataPacketFlag::saveForPlayer;
	this->slot = DataPacketSlot::movespeed;
	this->speed = speed;
} 