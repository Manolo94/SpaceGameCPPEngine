#pragma once

#include "Indicator.h"
#include "Game.h"

class IndicatorFuel : public Indicator
{
protected:
	Game* masterGame;
public:
	IndicatorFuel(float x, float y, float rotation, Game* game)
	{
		masterGame = game;
		RenderObject::Init(game->GetResourcesManager()->retrieveResource_ANIM(INDICATOR_FUEL), x, y, rotation, 100, 750, game, FRONT_LAYER);
		Indicator::indicatorFont = game->GetResourcesManager()->retrieveResource_FONT(STANDARD_FONT);
	};
	void virtual Render(bool debug = false, bool frozenFrame = false)	
	{
		if(masterGame->GetGameStat(PLAYER_FUEL) > 0) al_draw_filled_rectangle(Indicator::GetX() - rotationCenterX*0.5*scaleX, 
																				Indicator::GetY() - rotationCenterY*0.95*scaleY + Indicator::GetHeight()*(1-masterGame->GetGameStat(PLAYER_FUEL))*0.90, 
																				Indicator::GetX() + rotationCenterX*0.5*scaleX, 
																				Indicator::GetY() + rotationCenterY*0.95*scaleY, 
																				al_map_rgb(255, 0, 24));

		RenderObject::Render();
	};
};