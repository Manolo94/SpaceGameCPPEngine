#pragma once

#include "Indicator.h"
#include "Game.h"

class IndicatorCompass : public Indicator
{
protected:
	Game* masterGame;
public:
	IndicatorCompass(float x, float y, float rotation, Game* game)
	{
		masterGame = game;
		RenderObject::Init(game->GetResourcesManager()->retrieveResource_ANIM(INDICATOR_COMPASS), x, y, rotation, 120, 80, game, FRONT_LAYER);
		RenderObject::SetRotationCenter(RenderObject::GetWidth()/3, RenderObject::GetHeight()/2);
		Indicator::indicatorFont = game->GetResourcesManager()->retrieveResource_FONT(STANDARD_FONT);
	};
	void virtual Render(bool debug = false, bool frozenFrame = false)	
	 {
		RenderObject::SetRotation(masterGame->GetGameStat(PLAYER_ANGLE_TO_CENTER));

		RenderObject::Render();

		al_draw_textf(Indicator::indicatorFont, al_map_rgb(255,255,255), Indicator::GetX(), Indicator::GetY()-10, ALLEGRO_ALIGN_CENTRE, "%0.0f", masterGame->GetGameStat(PLAYER_DISTANCE_TO_CENTER));
	};
};