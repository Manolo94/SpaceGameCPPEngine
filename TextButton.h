#pragma once

#include "Button.h"

class TextButton : public Button
{
private:
	const char* text;

	//Render Colors
	ALLEGRO_COLOR textColor;
	ALLEGRO_COLOR fillColor;
	ALLEGRO_COLOR borderLineColor;

	//Label Font
	ALLEGRO_FONT* labelFont;

	//On Click Callback
	void(*onClickCallback)(Game* game);

public:
	TextButton();

	void Init(int x, int y, int width, int height, b2World* world, Game* game,
		ALLEGRO_COLOR textColor, ALLEGRO_COLOR fillColor, ALLEGRO_COLOR borderColor,
		const char* text, ALLEGRO_FONT* font);

	void SetOnClickCallback(void(*)(Game*));

	void OnClick();

	void TextButton::Render();
};

//class ExitButton : public TextButton
//{
//public:
//	ExitButton(int x, int y, int width, int height, b2World* world, Game* game,
//	ALLEGRO_COLOR textColor, ALLEGRO_COLOR fillColor, ALLEGRO_COLOR borderColor,
//	const char* text, ALLEGRO_FONT* font);
//
//	void ExitButton::OnClick();
//};