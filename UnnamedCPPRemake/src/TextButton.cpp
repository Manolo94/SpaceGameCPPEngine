#include "Game.h"
#include "TextButton.h"
#include "RenderObject.h"

TextButton::TextButton() : Button()
{
};

void TextButton::Init(int x, int y, int width, int height, b2World* world, Game* game,
	ALLEGRO_COLOR textColor, ALLEGRO_COLOR fillColor, ALLEGRO_COLOR borderColor,
	const char* text, ALLEGRO_FONT* font)
{
	Button::Init(NULL, x, y, width, height, world, game, true);

	TextButton::textColor = textColor;
	TextButton::fillColor = fillColor;
	TextButton::borderLineColor = borderColor;

	TextButton::text = text;
	TextButton::labelFont = font;

	onClickCallback = NULL;
};

void TextButton::SetOnClickCallback(void(*callback)(Game* masterGame))
{
	onClickCallback = callback;
}

void TextButton::OnClick()
{
	if (onClickCallback != NULL)
		onClickCallback(masterGame);
}

void TextButton::Render()
{
	Button::Render();

	// Compute values needed for rendering
	int buttonWidth = Button::GetRenderObject()->GetWidth();
	int buttonHeight = Button::GetRenderObject()->GetHeight();

	int buttonCenterX = Button::GetRenderObject()->GetX();
	int buttonCenterY = Button::GetRenderObject()->GetY();
	int buttonLeft = buttonCenterX - buttonWidth / 2;
	int buttonTop = buttonCenterY - buttonHeight / 2;
	int buttonRight = buttonCenterX + buttonWidth / 2;
	int buttonBottom = buttonCenterY + buttonHeight / 2+1;

	int t; int textWidth; int textHeight;
	al_get_text_dimensions(labelFont, text, &t, &t, &textWidth, &textHeight);

	// Get the text in the center
	int textLeft = buttonCenterX - textWidth / 2;
	int textTop = buttonCenterY - textHeight / 2;

	// Render
	al_draw_rectangle(buttonLeft, buttonTop, buttonRight, buttonBottom, 
		TextButton::borderLineColor, 1.00);

	if (GetButtonState() == Button::BUTTON_STATES::ON)
		al_draw_filled_rectangle(buttonLeft, buttonTop, buttonRight, buttonBottom, fillColor);

	al_draw_text(labelFont, textColor, textLeft, textTop, 0, text);
}