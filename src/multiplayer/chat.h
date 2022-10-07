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

class Window_Base;

#define max_chat_message_lines

class Message 
{	
	public:
	enum MessageType 
	{
		map,
		game,
		server,
		party,
		direct
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
	std::string inputString;
	std::vector<Message> messageLog;
	bool focus;
	static Chat chatInstance;
	public:

	Chat(std::shared_ptr<Scene> scene, int zoom);
	inline static Chat& Instance() { return chatInstance; }

	void SetFocus(bool focus);
	bool GetFocus();
	void PushMessage(const Message& Message);
	void Draw(Bitmap& dst);
	void Update();
	std::string& GetInputString();
	void FireMessage();
	void ProcessTextInput(SDL_Event);
	void ProcessKeyDown(SDL_Event);
};