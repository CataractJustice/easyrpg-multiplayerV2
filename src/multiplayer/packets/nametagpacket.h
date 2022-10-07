#include "datapacket.h"
#include <memory.h>
#include <string>
#define max_nametag_byte_length 32

struct INametagPacket : IDataPacket 
{
	char nametag[max_nametag_byte_length];
}

struct ONametagPacket : ODataPacket
{
	char nametag[max_nametag_byte_length];
	ONametagPacket(std::string name) 
	{
		memset(nametag, 0, max_nametag_byte_length);
		memcpy(nametag, name.c_str(), std::min(max_nametag_byte_length, (int)name.lenght()));
		this->flags = DataPacketFlag::saveForPlayer | DataPacketFlag::broadcastForServer;
	}
};
