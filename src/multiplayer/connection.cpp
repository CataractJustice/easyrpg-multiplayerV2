#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <iostream>
#include "connection.h"
#include "output.h"
#include "packets/basepacket.h"
#include "multiplayercharacter.h"
#include "packets/connectpacket.h"
#include "packets/inmappacket.h"
#include "packets/disconnectpacket.h"
#include "datapacketshandler.h"

ix::WebSocket webSocket;
#define MAX_PACKET_SIZE 1024
#define PACKETS_QUEUE_CAPACITY 32
unsigned char receiveBuffer[MAX_PACKET_SIZE * PACKETS_QUEUE_CAPACITY];
unsigned int receiveByteLength[PACKETS_QUEUE_CAPACITY];

unsigned int packetQueuePushCursor = 0;
unsigned int packetQueuePopCursor = 0;
unsigned int packetQueueSize = 0;

void onMessageAsync(unsigned char* data, unsigned int byteLength) 
{
	memcpy(receiveBuffer + MAX_PACKET_SIZE * packetQueuePushCursor, data, byteLength);
	receiveByteLength[packetQueuePushCursor] = byteLength;
	packetQueuePushCursor++;
	packetQueueSize++;
	packetQueueSize = packetQueueSize > PACKETS_QUEUE_CAPACITY ? PACKETS_QUEUE_CAPACITY : packetQueueSize;
	packetQueuePushCursor %= PACKETS_QUEUE_CAPACITY;
}

void Connect(std::string url, std::string game, Credentials credentials) 
{
	ix::initNetSystem();
	webSocket.setUrl(url);
	//Output::Debug("Multiplayer Setting WebSocket callbacks.");
	webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg)
	    {
	        if (msg->type == ix::WebSocketMessageType::Message)
	        {
				onMessageAsync((unsigned char*)msg->str.c_str(), msg->str.length());
	        }
	        else if (msg->type == ix::WebSocketMessageType::Open)
	        {
				//Output::Debug("Multiplayer WebSocket open.");
	        }
	        else if (msg->type == ix::WebSocketMessageType::Error)
	        {
				//Output::Warning("Multiplayer WebSocket error: {}", msg->errorInfo.reason);
	        } 
			else if(msg->type == ix::WebSocketMessageType::Close) 
			{
				//Output::Debug("Multiplayer WebSocket connection closed: {}.", msg->closeInfo.reason);
			}
	    }
	);

	//Output::Debug("Multiplayer Starting WebSocket.");
	webSocket.start();
}


void ConnectionUpdate() 
{
	while (packetQueueSize > 0) 
	{
		packetQueueSize--;
		unsigned char* data = receiveBuffer + MAX_PACKET_SIZE * packetQueuePopCursor;
		unsigned int byteLength  = receiveByteLength[packetQueuePopCursor];
		packetQueuePopCursor++;
		packetQueuePopCursor %= PACKETS_QUEUE_CAPACITY;

		onMessage(data, byteLength);
	}
}

void sendMessage(unsigned char* data, unsigned int byteLength) 
{
	webSocket.sendBinary(ix::IXWebSocketSendData((char*)data, byteLength));
}

void onMessage(unsigned char* data, unsigned int byteLength) 
{	
	std::string decmsg = "";
	for(int i = 0; i < byteLength; i++) 
	{
		decmsg += std::to_string((int)data[i]) + " ";
	}
	Output::Debug("Multiplayer message: {}", decmsg);
	IncomingPacket* packet = ((IncomingPacket*)data);
	switch (packet->type)
	{
	case IPacketType::connect:
	{
		ConnectPacket* connectPacket = (ConnectPacket*)packet;
		MultiplayerCharacter::MakeCharacter(connectPacket->id);
	}
		break;
	case IPacketType::inmap: 
	{
		InMapPacket* inMapPacket = (InMapPacket*)packet;
		MultiplayerCharacter::MakeCharacter(inMapPacket->id);
	}
	break;

	case IPacketType::disconnect:
	{
		DisconnectPacket* disconnectPacket = (DisconnectPacket*)packet;
		MultiplayerCharacter::RemovePlayer(disconnectPacket->id);
	}
	break;

	case IPacketType::mdata:
	{
		HandleDataPacket((IDataPacket*)data);
	}
	break;
	}
}

void ConnectionClose()
{
	webSocket.stop();
}