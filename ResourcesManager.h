#pragma once

#include "Globals.h"

class Animation;
struct ALLEGRO_FONT;

class ResourcesManager
{
private:
	Animation* AnimationResources[RESOURCE_ANIM_TOTAL];

	ALLEGRO_FONT* fontResources[1];

public:
	ResourcesManager();

	//Loading...
	bool loadAllResources();
	bool loadResource(Animation* anim, int place);
	bool loadResource(ALLEGRO_FONT* font, int place);

	//Retrieving...
	Animation* retrieveResource_ANIM(int place);
	Animation* retrieveResource_ANIM_piece(int place, float cutStartX, float cutEndX, float cutStartY, float cutEndY);
	ALLEGRO_FONT* retrieveResource_FONT(int place);

	//Destroying...
	void ClearAll();
};