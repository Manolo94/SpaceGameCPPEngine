#pragma once

#include "Globals.h"
#include <string>

class Animation;
struct ALLEGRO_FONT;

class ResourcesManager
{
private:
	char* RESOURCE_ANIM_REFERENCE_str[RESOURCE_ANIM_TOTAL] =
	{
		BASE_RESOURCES(GENERATE_STRING)
		COLLECTIBLE_RESOURCES(GENERATE_STRING)
		BACKGROUND_RESOURCES(GENERATE_STRING)
		INDICATOR_RESOURCES(GENERATE_STRING)
		BUTTON_RESOURCES(GENERATE_STRING)
		CURSOR_RESOURCES(GENERATE_STRING)
		ITEM_RESOURCES(GENERATE_STRING)
	};

	bool parse_line(const std::string& line, int line_number);
	bool load_resource_file(const std::string& filename);

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