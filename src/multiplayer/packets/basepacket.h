#pragma once
#pragma pack(1)

struct IPacketType
{
	enum Type
	{
		connect,
		inmap,
		disconnect,
		mdata
	};
};

struct OutgoingPacket
{
	unsigned char type;
};

#pragma pack(1)
struct IncomingPacket
{
	unsigned char type;
};
