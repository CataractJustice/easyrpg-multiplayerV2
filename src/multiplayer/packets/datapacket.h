#pragma once
#include "basepacket.h"
struct DataPacketFlag
{
	enum Flag {
	broadcastForMap = 1 << 0,//1
	broadcastForGame = 1 << 1,//2
	broadcastForServer = 1 << 2,//4
	saveForPlayer = 1 << 3,//8
	saveForMap = 1 << 4,//16
	saveForGame = 1 << 5,//32
	saveForServer = 1 << 6//64
	};
};

#define PIC_SLOTS 16
#define VAR_SLOTS 128
#define SWITCH_SLOTS 128
#define EVENT_SLOTS 128

#define SlotsRange(name, range) name##0, name##Last = name##0 + range - 1

struct DataPacketScope 
{
	enum Scope 
	{
		responce,
		personal,
		map,
		game,
		server
	};
};

struct DataPacketSlot
{
	enum Slot {
		position,
		facing,
		movespeed,
		sprite,
		sound,
		pflash,
		ptint,
		pflashStop,
		ptintStop,
		mflash,
		mtint,
		weather,
		jump,
		spritebitmap,
		SlotsRange(pic, PIC_SLOTS),
		SlotsRange(var, VAR_SLOTS),
		SlotsRange(switch, SWITCH_SLOTS),
		SlotsRange(event, EVENT_SLOTS),
		lastSlot
	};
};

struct IDataPacket : IncomingPacket
{
	unsigned char scope;
	unsigned int ownerId;
	unsigned short slot;
};


struct ODataPacket : OutgoingPacket
{
	unsigned char flags;
	unsigned short slot;
	ODataPacket();
};
