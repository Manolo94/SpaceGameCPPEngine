#include "Game.h"
#include "Camera.h"
#include "Player.h"

#include <allegro5/allegro_native_dialog.h>
#include "Box2D/Box2D.h"

//CLASS INITIATOR
Game::Game()
{
	Game::initialized = false;

	Game::gameState = NULL;
	GameStateID = -1;

	mainCamera = new Camera();
};

//Class initialization
void Game::Init(b2World *world)
{
	//Setting keyboard...
	keys.clear();

	//Debug
	//GameStateAddress = GAME_STATES::MAIN_MENU;
	//gameStates[GAME_STATES::MAIN_MENU] = new GameState_MainMenu(this);

	Game::initialized = true;
	Game::exiting = false;

	//ClearingGameStats
	for(int i = 0; i<GAME_STATS_TOTAL; i++) Game::GameStats[i] = 0.0;

	//Initiating ResourcesManager
	RMGame = new ResourcesManager();

	//Initiating Camera
	mainCamera->Init(0, 0, world, this);
};
void Game::StartGame()
{
	//Set State
	ChangeState(GAME_STATES::MAIN_MENU);
	ChangeState(GAME_STATES::RESOURCE_COLLECTION);

	//Create Main Menu RenderObject and hide
	//Create a b2World and GameObject Buttons inside Main Menu Class, 
	//change Main Menu State to update the render
	//

	//Putting the mouse
	mouse = new MouseObject(this);
	//Setting mouseState
	Collector_Default* state = new Collector_Default(mouse);
	mouse->ChangeMouseState(state);

	//Putting door
	Button *button = new Button();
	
	button->Init(GetResourcesManager()->retrieveResource_ANIM(BTN_MAINHATCH),
		0, 0, 70, 100, GetMainCamera()->focusedWorld, this);
	button->SetAnimation_DEFAULT(5, 0, -1, 0);
	button->SetAnimation_ON(0, 5, 1, 0);
	button->SetAnimation_CLICK(0, 2, -1, 1);

	main_menu = new MainMenu(GetMainCamera()->focusedWorld, this);
	
	//Creating Player...
	Game::player = new Astronaut(PLAYER_INIT_X, PLAYER_INIT_Y, 0, 0, GetMainCamera()->focusedWorld, this);
	//Max Fuel
	GameStats[PLAYER_FUEL] = 1.00;
	//Following Player
	mainCamera->Follow(player, 10);
};

bool Game::GetInit()
{
	if(!Game::initialized) al_show_native_message_box(al_get_current_display(), "Manolo Game Engine", "Game not initialized", NULL, NULL, ALLEGRO_MESSAGEBOX_ERROR);

	return initialized;
};

//Keyboard
//Add key
void Game::AddKey(int keycode)
{
	if (CheckKeyPressed(keycode)) return;
	keys.push_back(keycode);
}
//Check Key
bool Game::CheckKeyPressed(int keycode)
{
	std::list<int>::iterator keyIter;
	for (keyIter = keys.begin(); keyIter != keys.end(); keyIter++)
	{
		if ((*keyIter) == keycode) return true;
	}
	return false;
}
//Remove Key
void Game::removeKey(int keycode)
{
	keys.remove(keycode);
}

//STATE MACHINE
void Game::ChangeState(int newGameState)
{
	if(gameState != NULL) gameState->EndState();
	GameStateID = newGameState;
	gameState = CreateGameState(newGameState);
	if(gameState != NULL) gameState->StartState();
};

GameState* Game::CreateGameState(int gameState)
{
	switch(gameState)
	{
	case GAME_STATES::RESOURCE_COLLECTION:
		return new GameState_ResourceCollection(this);
	case GAME_STATES::MAIN_MENU:
		return new GameState_MainMenu(this);
	}
}

int Game::GetGameState()
{
	return GameStateID;
}

//SpaceCollectible FUNCTIONS
//Game Functions
void Game::ThrowSpaceCollectibles(ResourcesManager* RM, b2World *world, float focusX, float focusY)
{
	if(!Game::GetInit()) return;
	if(GetMainCamera()->GameObjectList.size() > 900) return;

	float locationAngle = GeneralFunctions::randRange(0, 2*3.141, 3);

	if (GeneralFunctions::randRange(0, 4) == 4)
		new SpaceCollectible_Column(RM->retrieveResource_ANIM(COLLECTIBLE_COLUMN), focusX*mToPx + cos(locationAngle)*WIDTH, focusY*mToPx + sin(locationAngle)*WIDTH,
		GeneralFunctions::randRange(100, 300), GeneralFunctions::randRange(-6, 6, 3), GeneralFunctions::randRange(-6, 6, 3),
		world, this);
	else
		new SpaceCollectible_Asteroid(RM->retrieveResource_ANIM(COLLECTIBLE_ASTEROID), focusX*mToPx + cos(locationAngle)*WIDTH, focusY*mToPx + sin(locationAngle)*WIDTH, 
		GeneralFunctions::randRange(10, 100), GeneralFunctions::randRange(-6, 6, 3), GeneralFunctions::randRange(-6, 6, 3),
		world, this);
	
};

void Game::UpdateMouse(float mouseX, float mouseY)
{
	mouse->Update(mouseX, mouseY, (Game::GetMainCamera()->focusX) - WIDTH/2/mToPx, (Game::GetMainCamera()->focusY) - HEIGHT/2/mToPx);
};

//UPDATE
void Game::Update(float mouseX, float mouseY)
{
	//Update world
	GetMainCamera()->focusedWorld->Step(*GetMainCamera()->GetTimeStep(),
		*GetMainCamera()->GetVelocityIterations(),
		*GetMainCamera()->GetPositionIterations());

	//Update Game
	if (gameState == NULL) return;
	gameState->UpdateState();

	//Update Camera
	Game::GetMainCamera()->Update(mouseX, mouseY);

	//Update mouse
	Game::UpdateMouse(mouse->GetX(), mouse->GetY());
};

//RENDER
void Game::Render()
{
	if(!Game::GetInit()) return;
	Game::GetMainCamera()->Render();

	// TODO: Implement a UI system
	al_draw_text(GetResourcesManager()->retrieveResource_FONT(STANDARD_FONT), 
					al_map_rgb(255, 255, 255), 300, 100, 0, "Right click to use thruster");
	al_draw_text(GetResourcesManager()->retrieveResource_FONT(STANDARD_FONT),
					al_map_rgb(255, 255, 255), 300, 120, 0, "Get close to asteroids and left click on them to attach rocket");
	al_draw_text(GetResourcesManager()->retrieveResource_FONT(STANDARD_FONT),
					al_map_rgb(255, 255, 255), 300, 140, 0, "Esc for menu");
};