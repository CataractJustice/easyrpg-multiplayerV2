#include "multiplayercharacter.h"
#include "main_data.h"
#include "output.h"
#include "drawable_mgr.h"
#include "scene_map.h"
#include "main_data.h"
#include "game_system.h"
#include "chat.h"

#define MOVE_QUEUE_CAPACITY 8

void MultiplayerCharacter::MultiplayerMove(int x, int y) 
{
	this->moveQueue.push(std::pair<int, int>(x, y));
	if(moveQueue.size() > MOVE_QUEUE_CAPACITY)
		moveQueue.pop();
}

void MultiplayerCharacter::Update() 
{
	SetProcessed(false);
	Game_Character::Update();
	this->sprite->Update();

	if(this->IsStopping() && !moveQueue.empty()) 
	{
		std::pair<int, int> nextMove = moveQueue.front();
		moveQueue.pop();

		int dx = nextMove.first - GetX();
		int dy = nextMove.second - GetY();

		dx += 1;
		dy += 1;

		if(dx >= 0 && dy >= 0 && dx <= 2 && dy <= 2) {

			const Direction directons[3][3] = 
			{
				{Direction::UpLeft, 	Direction::Up, 		Direction::UpRight		},
				{Direction::Left, 		Direction(-1), 		Direction::Right		},
				{Direction::DownLeft,	Direction::Down, 	Direction::DownRight	}
			};

			Direction direction = directons[dy][dx];
			if(direction != -1)
				Move(direction);
		} 
		else 
		{
			SetX(nextMove.first);
			SetY(nextMove.second);
		}
	}
}

void MultiplayerCharacter::UpdateNextMovementAction() {};

MultiplayerCharacter::MultiplayerCharacter(unsigned int id) : Game_PlayerBase(MPlayer)
{
	this->nametag = "Player["+std::to_string(id)+"]";
	this->id = id;
	this->SetSpriteGraphic(Main_Data::game_player->GetSpriteName(), Main_Data::game_player->GetSpriteIndex());
	this->SetX(Main_Data::game_player->GetX());
	this->SetY(Main_Data::game_player->GetY());
	this->SetLayer(Main_Data::game_player->GetLayer());
	this->SetMoveSpeed(Main_Data::game_player->GetMoveSpeed());
	this->SetMoveFrequency(Main_Data::game_player->GetMoveFrequency());
	this->SetThrough(true);
	auto old_list = &DrawableMgr::GetLocalList();
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
	sprite = std::make_unique<Sprite_Character>(this);
	DrawableMgr::SetLocalList(old_list);
	Output::Debug("New Multiplayer Character Created, id: {}.", id);
}

void MultiplayerCharacter::MakeCharacter(unsigned int id) 
{
	characters[id] = std::make_shared<MultiplayerCharacter>(id);
}

void MultiplayerCharacter::UpdateCharacters() 
{
	for(auto map_pair : characters) 
	{
		map_pair.second->Update();
	}
}

void MultiplayerCharacter::RemovePlayer(unsigned int id) 
{
	auto characterIterator = characters.find(id);
	if(characterIterator == characters.cend()) 
	{
		Output::Warning("Trying to remove non existing multiplayer character with id: {}", id);
		return;
	}

	auto old_list = &DrawableMgr::GetLocalList();
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
	characters.erase(characterIterator);
	DrawableMgr::SetLocalList(old_list);
	Output::Debug("Removed player, id: {}.", id);
}

std::shared_ptr<MultiplayerCharacter> MultiplayerCharacter::GetPlayer(unsigned int id) 
{
	auto characterIterator = characters.find(id);
	if(characterIterator == characters.cend()) 
		return std::shared_ptr<MultiplayerCharacter>(nullptr);
	
	return (*characterIterator).second;
}

void MultiplayerCharacter::ClearCharacters() 
{
	characters.clear();
}

float MultiplayerCharacter::GetDistanceToPlayer() 
{
	auto& player = Main_Data::game_player;
	auto& map = Game_Map::GetMap();

	float dx1 = player->GetX() - this->GetX();
	float dy1 = player->GetY() - this->GetY();
	
	float dx2 = player->GetX() - (this->GetX() - map.width);
	float dy2 = player->GetY() - (this->GetY() - map.height);

	float dx = std::min(dx1, dx2);
	float dy = std::min(dy1, dy2);

	return std::sqrt(dx * dx + dy * dy);
}

void MultiplayerCharacter::PlaySound(lcf::rpg::Sound sound) 
{
	float l = GetDistanceToPlayer();

	sound.volume = sound.volume * MultiplayerCharacter::soundVolume * std::max(1.0f - l / MultiplayerCharacter::soundFalloffDistance, 0.0f);

	Main_Data::game_system->SePlay(sound);
}

void MultiplayerCharacter::FlashAll(int r, int g, int b, int power, int frames) 
{
	for(auto pair : characters) 
	{
		pair.second->Flash(r, g, b, power, frames);
	}
}


std::map<unsigned int, std::shared_ptr<MultiplayerCharacter>> MultiplayerCharacter::characters = std::map<unsigned int, std::shared_ptr<MultiplayerCharacter>>();

float MultiplayerCharacter::soundVolume = 1.0f;
float MultiplayerCharacter::soundFalloffDistance = 10.0f;

void MultiplayerCharacter::DisplayNametag(BitmapRef& surface, int zoom)
{
	int x = GetScreenX() - TILE_SIZE / 3;
	int y = GetScreenY() - TILE_SIZE - TILE_SIZE;
	Text::Draw(*surface, x * zoom, y * zoom, *Font::Default(), Color(255,255,255,255), this->nametag);
}

void MultiplayerCharacter::FullresPass(BitmapRef& surface, int zoom) 
{
	for(auto pair : characters) 
	{
		pair.second->DisplayNametag(surface, zoom);
	}
}

std::unique_ptr<Chat> chat;

void FullresDrawPass(BitmapRef& display_surface, int zoom) 
{
	MultiplayerCharacter::FullresPass(display_surface, zoom);
	if(!chat) 
	{
		auto scene_map = Scene::Find(Scene::SceneType::Map);
		if(scene_map != nullptr) {
			chat = std::make_unique<Chat>(scene_map, zoom);
			chat->PushMessage(Message(0, Message::server, "Char0x61", "My Love For Hatsune Miku Is Constant And Sharp. And I Wish For A Better World For Everyone. In fact, I Want Miku To Be Inflicted On Others. I Want No One To Escape.", 16, zoom));
			chat->PushMessage(Message(0, Message::server, "Char0x61", "AAAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 16, zoom));
			chat->PushMessage(Message(0, Message::server, "Char0x61", "Test test test", 16, zoom));
			chat->PushMessage(Message(0, Message::server, "Char0x61", "Blah blah blah", 16, zoom));
			chat->PushMessage(Message(0, Message::server, "Char0x61", "Жопа", 16, zoom));
			chat->PushMessage(Message(0, Message::server, "Char0x61", "Shittin N Fartin Shittin N Fartin", 16, zoom));
		}
	}
	if(chat) chat->Draw(*display_surface);
}