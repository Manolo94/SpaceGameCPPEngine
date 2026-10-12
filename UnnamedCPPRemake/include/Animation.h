#pragma once

#include <allegro5/allegro5.h>

class Animation
{
	/*Only the Resource Manager can access protected functions of Animation.
	Therefore only ResourceManager can create it.
	It prevents leaking bitmaps*/
	friend class ResourcesManager;
protected:
	Animation(const char* filename,
		float frameWidth, float frameHeight,
		int maxFrame = 1, int frameDelay = 1,
		int animationColumns = 1);
	Animation(ALLEGRO_BITMAP* image,
		float frameWidth, float frameHeight,
		int maxFrame = 1, int frameDelay = 1,
		int animationColumns = 1);
	Animation();
public:
	int maxFrame;
	int frameDelay;
	float frameWidth;
	float frameHeight;
	int animationColumns;

	ALLEGRO_BITMAP *animationImage;
};