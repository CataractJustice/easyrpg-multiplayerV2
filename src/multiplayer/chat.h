#pragma once
#include <memory>
#include "options.h"
#include "scene.h"
#include "bitmap.h"
#include "window_base.h"
#include <string>

extern "C" 
{
	union SDL_Event;
}

struct IChatMsgPacket;

class Window_Base;

class Message 
{	
	public:
	enum MessageType 
	{
		map,
		game,
		server,
		direct,
		party
	};

	private:
	MessageType type;
	std::string nametag;
	std::string text;
	int ownerId;

	public:

	

	Message(int ownerId, MessageType type, const std::string& nametag, const std::string& text, int maxLines, int zoom);

	BitmapRef messageBitmap;
};

class Chat 
{
	private:
	std::unique_ptr<Window_Base> window;
	int zoom;
	int scroll;
	int logHeight;
	bool needsRefresh;
	BitmapRef messageLogBitmap;
	int cursor;
	std::string inputString;
	std::unique_ptr<Window_Base> inputWindow;
	int inputScroll;
	std::vector<Message> messageLog;
	bool focus;
	static Chat* chatInstance;
	public:

	Chat(std::shared_ptr<Scene> scene, int zoom);
	static Chat& Instance();
	static bool HasInstance();
	static void MakeInstance(std::shared_ptr<Scene> scene, int zoom);

	int Zoom();

	void SetFocus(bool focus);
	bool GetFocus();
	void PushMessage(const Message& message);
	void PushMessage(IChatMsgPacket* message);
	void Draw(Bitmap& dst);
	void Update();
	void RefreshInput();

	
	std::string& GetInputString();
	void FireMessage();
	void ProcessTextInput(SDL_Event);
	void ProcessKeyDown(SDL_Event);
};