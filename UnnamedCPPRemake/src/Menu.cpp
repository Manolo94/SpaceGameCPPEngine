#include "Menu.h"
#include "ResourcesManager.h"
#include "TextButton.h"
#include "Game.h"

Menu::Menu()
{

}

void Menu::Init(float xPosition, float yPosition, float width, float height, char alignment, b2World* world, Game* game)
{
	menuWorld = world;
	masterGame = game;
	Menu::alignment = alignment;

	RenderObject::Init(NULL, xPosition, yPosition, rotation, width, height, masterGame, LAYERS::FRONT_LAYER);
}

void Menu::Render(bool debug, bool frozenFrame)
{
	if (masterGame->GetGameState() == GAME_STATES::MAIN_MENU)
	{
		for (int i = 0; i < menuButtons.size(); i++)
			menuButtons.at(i)->Render();
	}
}

void Exit(Game* masterGame)
{
	if (masterGame->GetGameState() == GAME_STATES::MAIN_MENU)
	{
		printf("EXIT GAME");
		masterGame->ExitGame();
	}
}

void Options(Game* masterGame)
{
	if (masterGame->GetGameState() == GAME_STATES::MAIN_MENU)
	{
		printf("OPTIONS");
	}
}

void Resume(Game* masterGame)
{
	if (masterGame->GetGameState() == GAME_STATES::MAIN_MENU)
	{
		printf("RESUME");
		masterGame->ChangeState(GAME_STATES::RESOURCE_COLLECTION);
	}
}

void Menu::addButton(char* text, void (*callback)(Game*), int position)
{
	// TODO: handle alignment
	// TODO: height is too static

	TextButton* newB;

	int menuTop = Menu::x - Menu::height / 2;

	// Set the height depending on how many elements there are
	newB = new TextButton();
	newB->Init(Menu::x, menuTop + 35*position, 100, 30, menuWorld, masterGame,
		       al_map_rgb(0, 200, 50), al_map_rgba(0, 200, 50, 100), al_map_rgb(0, 200, 50),
		       text, masterGame->GetResourcesManager()->retrieveResource_FONT(STANDARD_FONT));

	// Set OnClick
	newB->SetOnClickCallback(callback);

	menuButtons.push_back(newB);
}

MainMenu::MainMenu(b2World* world, Game* game)
{
	Menu::Init(WIDTH/2, HEIGHT/2, 400, 600, 'c', world, game);

	Menu::addButton("EXIT", Exit, 3);
	Menu::addButton("OPTIONS", Options, 2);
	Menu::addButton("RESUME", Resume, 1);
}