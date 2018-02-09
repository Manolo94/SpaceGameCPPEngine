#pragma once
#include "RenderObject.h"
#include <vector>

class b2World;
class Button;

class Menu : public RenderObject
{
private:
	Game* masterGame;
	b2World* menuWorld;
	char alignment;

	std::vector <Button*> menuButtons;
public:

	enum MENU_ACTIONS{RESUME, OPTIONS, EXIT_GAME};

	Menu();

	void Menu::Init(float xPosition, float yPosition, float width, float height, char alignment, b2World* world, Game* game);

	void virtual Render(bool debug = false, bool frozenFrame = false);

	void addButton(char* text, void(*)(Game*), int position);
};

class MainMenu : public Menu
{
private:

public:
	MainMenu::MainMenu(b2World* world, Game* game);
};