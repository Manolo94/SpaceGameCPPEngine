#pragma once

#include "State.h"
#include "Menu.h"

class Game;

class GameState : public State
{
private:
	Game *masterGame;

public:
	void Init(Game* masterGame)
	{
		GameState::masterGame = masterGame;
	};

	//Reader
	Game *GetMasterGame() { return masterGame; };
};

class GameState_MainMenu : public GameState
{
private:
	bool readyForResume;
	Menu* main_menu;

public:

	GameState_MainMenu(Game* masterGame);

	void StartState();
	void UpdateState();
	void EndState();
};

class GameState_ResourceCollection : public GameState
{
private:
	bool readyForPause;
public:
	GameState_ResourceCollection(Game* masterGame)
	{
		GameState::Init(masterGame);
		readyForPause = false;
	};

	void StartState();
	void UpdateState();
	void EndState();
};