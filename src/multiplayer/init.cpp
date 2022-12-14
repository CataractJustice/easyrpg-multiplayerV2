#include "init.h"
#include "connection.h"
#include "output.h"
#include "packets/mappacket.h"
#include "multiplayer/multiplayercharacter.h"

void MultiplayerStart() 
{
	Output::Debug("Multiplayer start");
	Connect("ws://yumenikki.online:7777", "Yume Nikki", Credentials());
}

void ChangeMap(int map) 
{
	sendPacket(MapPacket(map));
	MultiplayerCharacter::ClearCharacters();
}

void MultiplayerUpdate() 
{
	ConnectionUpdate();
	MultiplayerCharacter::UpdateCharacters();
}

void MultiplayerClose() 
{
	ConnectionClose();
}