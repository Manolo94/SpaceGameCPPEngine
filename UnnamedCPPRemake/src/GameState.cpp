#include "Game.h"
#include "Menu.h"


GameState_MainMenu::GameState_MainMenu(Game* masterGame)
{
	GameState::Init(masterGame);
	readyForResume = false;

	GameState_MainMenu::main_menu = new MainMenu(masterGame->GetMainCamera()->focusedWorld, masterGame);
};

void GameState_MainMenu::StartState()
{
	readyForResume = false;
	GetMasterGame()->GetMainCamera()->SetTimeStep(0);
};

void GameState_MainMenu::UpdateState()
{ 
	if (!GetMasterGame()->CheckKeyPressed(ALLEGRO_KEY_P)) readyForResume = true;
	if (readyForResume && GetMasterGame()->CheckKeyPressed(ALLEGRO_KEY_P)) GetMasterGame()->ChangeState(GAME_STATES::RESOURCE_COLLECTION); 
};

void GameState_MainMenu::EndState(){};

void GameState_ResourceCollection::StartState() 
{
	readyForPause = false;
	GetMasterGame()->GetMainCamera()->SetTimeStep(1.0f / 60.0f); 
};

void GameState_ResourceCollection::UpdateState()
{
	//KEYBOARD FUNCTIONS
	/*if (masterGame->keys[LEFT] == true) 
		GetMasterGame()->GetMainCamera()->MoveTo(GetMainCamera()->focusX - 0.1, GetMainCamera()->focusY, 1);
	if (keys[RIGHT] == true) GetMainCamera()->MoveTo(GetMainCamera()->focusX + 0.1, GetMainCamera()->focusY, 1);
	if (keys[UP] == true) GetMainCamera()->MoveTo(GetMainCamera()->focusX, GetMainCamera()->focusY - 0.1, 1);
	if (keys[DOWN] == true) GetMainCamera()->MoveTo(GetMainCamera()->focusX, GetMainCamera()->focusY + 0.1, 1);

	*/
	if (!GetMasterGame()->GetInit()) return;
	if (!GetMasterGame()->CheckKeyPressed(ALLEGRO_KEY_P)) readyForPause = true;
	if (readyForPause && GetMasterGame()->CheckKeyPressed(ALLEGRO_KEY_P)) GetMasterGame()->ChangeState(GAME_STATES::MAIN_MENU);
	if (GeneralFunctions::randRange(0, 20) == 0)
		GetMasterGame()->ThrowSpaceCollectibles(GetMasterGame()->GetResourcesManager(), 
												GetMasterGame()->GetMainCamera()->focusedWorld,
												GetMasterGame()->GetMainCamera()->focusX, 
												GetMasterGame()->GetMainCamera()->focusY);
};

void GameState_ResourceCollection::EndState() {};