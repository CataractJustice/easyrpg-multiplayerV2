#include "game_player.h"
#include "sprite_character.h"
#include <map>
#include <queue>


class MultiplayerCharacter : public Game_PlayerBase
{
	private:
	unsigned int id;
	std::string nametag;
	std::queue<std::pair<int, int>> moveQueue;
	public:
	std::unique_ptr<Sprite_Character> sprite;
	MultiplayerCharacter(unsigned int id);

	static void MakeCharacter(unsigned int id);

	void MultiplayerMove(int x, int y);

	void UpdateNextMovementAction() override;

	void Update();

	float GetDistanceToPlayer();

	void PlaySound(lcf::rpg::Sound sound);

	void DisplayNametag(BitmapRef& surface, int zoom);

	static void UpdateCharacters();

	static void FullresPass(BitmapRef& surface, int zoom);
	
	static void ClearCharacters();
	
	static void RemovePlayer(unsigned int id);

	static void FlashAll(int r, int g, int b, int power, int frames);

	static void TintAll(int r, int g, int b, int power, int frames);
	
	static std::shared_ptr<MultiplayerCharacter> GetPlayer(unsigned int id);
	
	static std::map<unsigned int, std::shared_ptr<MultiplayerCharacter>> characters;

	static float soundFalloffDistance;
	static float soundVolume;
};