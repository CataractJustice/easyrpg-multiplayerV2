#pragma once
#include <string>
#include "credentials.h"

void Connect(std::string url, std::string game, Credentials credentials);
void ConnectionUpdate();
void onMessage(unsigned char* data, unsigned int byteLength);
void sendMessage(unsigned char* data, unsigned int byteLength);
template<class T>
void sendPacket(const T& packet) 
{
	sendMessage((unsigned char*)&packet, sizeof(T));
}
void ConnectionClose();