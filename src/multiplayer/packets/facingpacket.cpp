#include "facingpacket.h"

OFacingPacket::OFacingPacket(unsigned char facing) 
{
	this->flags = DataPacketFlag::broadcastForMap, DataPacketFlag::saveForServer;
	this->facing = facing;
	this->slot = DataPacketSlot::facing;
}