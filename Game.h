#pragma once

#include <list>
#include <cmath>

//**************Resource Collection Section************
#include "Astronaut.h"
#include "SpaceCollectible_Asteroid.h"
#include "SpaceCollectible_Column.h"
#include "CollectorRocket.h"

//*******************Internal Management***************
#include "ResourcesManager.h"
#include "GameState.h"

#include "Camera.h"
#include "Background.h"
#include "GameObject.h"
#include "Animation.h"
#include "RenderObject.h"

//*******************User Interface********************
#include "Menu.h"
#include "Button.h"

#include "MouseObject.h"
#include "MouseState.h"
//**********************END #INCLUDE*******************

class b2World;
class Player;

class Game
{
protected:
	//Game State
	GameState *gameState;

	//Game Variables
	bool initialized;
	bool exiting;

	float GameStats[GAME_STATS_TOTAL];

	//Internal management
	ResourcesManager *RMGame;
	Camera *mainCamera;
	MouseObject *mouse;
	Player *player;

	MainMenu* main_menu;

public:
	Game();

	//Keyboard, up to five keys at the same time
	std::list<int> keys;

	//Readers
	bool GetInit();
	Player *GetPlayer() 
	{
		if(Game::GetInit())
		{
			return player;
		};
		return NULL;
	};
	Camera *GetMainCamera() 
	{
		if(Game::GetInit())
		{
			return mainCamera;
		};
		return NULL;
	};
	MouseObject *GetMouse() 
	{
		if(Game::GetInit())
		{
			return mouse;
		};
		return NULL;
	};
	ResourcesManager *GetResourcesManager()
	{
		if(Game::GetInit())
		{
			return RMGame;
		};
		return NULL;
	};


	//Initialization
	void Game::Init(b2World *world);

	void Game::StartGame();

	//Methods
	void ThrowSpaceCollectibles(ResourcesManager* RM, b2World *world, float focusX, float focusY);

	//Writters
	void SetGameStat(float value, int GameStatID)
	{
		Game::GameStats[GameStatID] = value;
	};
	float GetGameStat(int GameStatID)
	{
		return Game::GameStats[GameStatID];
	};

	//Keyboard
	//Add key
	void AddKey(int keycode);
	//Check Key
	bool CheckKeyPressed(int keycode);
	//Remove Key
	void removeKey(int keycode);

	//STATE MACHINE
	//Change the state
	int GameStateID;
	void ChangeState(int newGameState);
	GameState* CreateGameState(int gameState);
	int GetGameState();

	void UpdateMouse(float mouseX, float mouseY);

	void Update(float mouseX, float mouseY);

	void Render();

	void ExitGame()
	{
		exiting = true;
	}
	bool GetExit()
	{
		return exiting;
	}
};