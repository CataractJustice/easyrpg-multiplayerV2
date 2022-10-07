#pragma once
#include <lcf/rpg/sound.h>
#include "datapacket.h"
#include <memory.h>
#include <string>

#define max_sound_packet_sound_name_length 64

struct ISoundPacket : IDataPacket
{
	unsigned char volume;
	unsigned char balance;
	unsigned char tempo;
	char soundname[max_sound_packet_sound_name_length];
};

struct OSoundPacket : ODataPacket
{
	unsigned char volume;
	unsigned char balance;
	unsigned char tempo;
	char soundname[max_sound_packet_sound_name_length];
	OSoundPacket(lcf::rpg::Sound sound) 
	{
		this->flags = DataPacketFlag::broadcastForMap;

		memset(this->soundname, 0, max_sound_packet_sound_name_length);
		memcpy(this->soundname, sound.name.c_str(), std::min((int)sound.name.length(), max_sound_packet_sound_name_length));

		this->soundname[max_sound_packet_sound_name_length - 1] = 0;
		this->volume = sound.volume;
		this->balance = sound.balance;
		this->tempo = sound.tempo;

		this->slot = DataPacketSlot::sound;
	} 
};
