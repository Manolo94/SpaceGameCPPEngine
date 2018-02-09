#include "ResourcesManager.h"
#include <allegro5\allegro_font.h>
#include "Animation.h"

ResourcesManager::ResourcesManager()
{
	for(int i = 0; i < RESOURCE_ANIM_TOTAL; i++)
	{
		AnimationResources[i] = NULL;
	};
	for(int i = 0; i < RESOURCE_FONT_TOTAL; i++)
	{
		fontResources[i] = NULL;
	};
	AnimationResources[NULL_ANIM] = new Animation();
};


bool ResourcesManager::loadAllResources()
{
	//Loading Resources
	if (!loadResource(al_load_font("arial.ttf", 18, 0), STANDARD_FONT) ||
		!loadResource(new Animation("images/astronautGraph/astroUp.png", 30, 74), ASTRONAUT_BODY) ||
		!loadResource(new Animation("images/astronautGraph/fireAnim.png", 35, 50, 10, 2, 5), FX_THRUST_FIRE) ||
		!loadResource(new Animation("images/astronautGraph/pistol.png", 80, 20), ASTRONAUT_PISTOL) ||
		!loadResource(new Animation("images/SpaceCollectiblesGraph/asteroid.png", 420, 420), COLLECTIBLE_ASTEROID) ||
		!loadResource(new Animation("images/SpaceCollectiblesGraph/column.png", 643, 79), COLLECTIBLE_COLUMN) ||
		!loadResource(new Animation("images/outer-space.jpg", 1000, 750), BACKGROUND_1) ||
		!loadResource(new Animation("images/compassIndicator.png", 120, 80), INDICATOR_COMPASS) ||
		!loadResource(new Animation("images/fuelIndicator.png", 100, 600), INDICATOR_FUEL) ||
		!loadResource(new Animation("images/cursor.png", 50, 50), CURSOR) ||
		!loadResource(new Animation("images/rocketCursor.png", 50, 50), CURSOR_ROCKET) ||
		!loadResource(new Animation("images/rocketEngine.png", 55, 35), ROCKET_ENGINE) ||
		!loadResource(new Animation("images/button_MainHatch.png", 70, 100, 6, 4, 6), BTN_MAINHATCH))
		return false;
	else return true;

	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	/*if(!thisGame.GetResourcesManager()->loadResource(new Animation("images/collectResourcesMap.png", 12000, 12000), BACKGROUND_MAP))
	{
	al_show_native_message_box(display, "Manolo's Engine", "The engine couldn't find the image for the following entity:", "BACKGROUND_MAP", NULL, NULL);
	return -1;
	};*/
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
};

bool ResourcesManager::loadResource(Animation *anim, int place)
{
	if(anim->animationImage == NULL) return false;
	AnimationResources[place] = anim;

	return true;
};

bool ResourcesManager::loadResource(ALLEGRO_FONT* font, int place)
{
	if(font == NULL) return false;
	fontResources[place] = font;

	return true;
};

//Retrieving...
Animation* ResourcesManager::retrieveResource_ANIM(int place)
{
	return AnimationResources[place];
};
Animation* ResourcesManager::retrieveResource_ANIM_piece(int place, float cutStartX, float cutEndX, float cutStartY, float cutEndY)
{
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);

	ALLEGRO_BITMAP* piece = al_clone_bitmap(al_create_sub_bitmap(AnimationResources[place]->animationImage, cutStartX, cutStartY, cutEndX-cutStartX, cutEndY-cutStartY));

	return new Animation(piece, cutEndX-cutStartX, cutEndY-cutStartY);
};
ALLEGRO_FONT* ResourcesManager::retrieveResource_FONT(int place)
{
	return fontResources[place];
};

//Destroying...
void ResourcesManager::ClearAll()
{
	for(int i = 0; i < RESOURCE_ANIM_TOTAL; i++)
	{
		if(AnimationResources[i] != NULL) al_destroy_bitmap(AnimationResources[i]->animationImage);
	};
	for(int i = 0; i < 1; i++)
	{
		if(fontResources[i] != NULL) al_destroy_font(fontResources[i]);
	};
};