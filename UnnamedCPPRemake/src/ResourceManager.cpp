#include "ResourcesManager.h"
#include <allegro5\allegro_font.h>
#include "Animation.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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

bool readPath(std::istringstream& iss, std::string& resultPath)
{
	// Read the rest of the line as path (including spaces or slashes)
	std::getline(iss, resultPath);

	// Trim leading whitespace from path
	size_t start = resultPath.find_first_not_of(" \t");
	if (start != std::string::npos) {
		resultPath = resultPath.substr(start);
	}
	else {
		resultPath.clear();  // No path found
		return false;
	}

	return true;
}

bool ResourcesManager::parse_line(const std::string& line, int line_number) {
	std::istringstream theStream(line);
	std::string resultType, resultID, resultPath;
	if (!(theStream >> resultType >> resultID)) {
		return false;  // Failed to get first two words
	}

	// Parse the type
	if (resultType == "FONT")
	{
		int fontID = 0;
		// TODO: Setup font enums just like the animation ones
		if (resultID == "STANDARD_FONT")
		{
			fontID = STANDARD_FONT;
		}
		else
		{
			std::cerr << "Unrecognized ID on line " << line_number << ": " << line << "\n";
			return false;
		}

		int fontSize, fontFlags;
		if (!(theStream >> fontSize >> fontFlags)) 
		{
			std::cerr << "Missing fontsize and/or flags on line " << line_number << ": " << line << "\n";
			return false;
		}

		if (!readPath(theStream, resultPath))
		{
			std::cerr << "Could not read font path on " << line_number << ": " << line << "\n";
			return false;
		}

		bool result = loadResource(al_load_font(resultPath.c_str(), fontSize, fontFlags), fontID);

		if (!result)
		{
			std::cerr << "Failed to load resource on line " << line_number << ": " << line << "\n";
			return false;
		}
	}
	else if (resultType == "ANIM")
	{
		int animID = 0;

		// Find which animation enum string matches the given ID
		bool found = false;
		for (int i = 0; i < RESOURCE_ANIM_TOTAL; i++)
		{
			if (RESOURCE_ANIM_REFERENCE_str[i] == resultID)
			{
				found = true;
				animID = i;
			}
		}

		if (!found)
		{
			std::cerr << "Unrecognized ID on line " << line_number << ": " << line << "\n";
			return false;
		}

		int frameWidth, frameHeight, maxFrame, frameDelay, animationCols;
		if (!(theStream >> frameWidth >> frameHeight >> maxFrame >> frameDelay >> animationCols))
		{
			std::cerr << "Missing animation properties on line " << line_number << ": " << line << "\n";
			return false;
		}

		if (!readPath(theStream, resultPath))
		{
			std::cerr << "Could not read font path on " << line_number << ": " << line << "\n";
			return false;
		}

		bool result = loadResource(new Animation(resultPath.c_str(), frameWidth, frameHeight,
												 maxFrame, frameDelay, animationCols), animID);

		if (!result)
		{
			std::cerr << "Failed to load resource on line " << line_number << ": " << line << "\n";
			return false;
		}
	}
	else
	{
		std::cerr << "Unrecognized type on line " << line_number << ": " << line << "\n";
		return false;
	}

	return true;
}

// Reads a file, parses each line and loads the corresponding resource accordingly
bool ResourcesManager::load_resource_file(const std::string& filename) {
	std::ifstream file(filename);
	if (!file) 
	{
		std::cerr << "Failed to open file: " << filename << "\n";
		return false;
	}

	std::string line;
	int line_number = 0;
	while (std::getline(file, line)) 
	{
		++line_number;
		// Skip blank or comment lines
		if (line.empty() || line[0] == '#') continue;

		if (!parse_line(line, line_number))
		{
			std::cerr << "Parse error on line " << line_number << ": " << line << "\n";
			return false;
		}
	}

	return true;
}

bool ResourcesManager::loadAllResources()
{

	// TODO: Return the error to be shown in a dialog box
	//Loading Resources
	if(!load_resource_file("resources.txt"))
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