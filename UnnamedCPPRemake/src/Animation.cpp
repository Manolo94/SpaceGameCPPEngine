#include "Animation.h"
#include "GameObject.h"
#include "ResourcesManager.h"

#include <cstdio>

Animation::Animation(const char* filename, 
		float frameWidth, float frameHeight, 
		int maxFrame, int frameDelay,
		int animationColumns)
{
	if(!(Animation::animationImage = al_load_bitmap(filename)))
		printf("%s couldn't find the file '%s'!!!", "Onion ENgine\n", filename);
	Animation::maxFrame = maxFrame;
	Animation::frameDelay = frameDelay;

	Animation::frameWidth = frameWidth;
	Animation::frameHeight = frameHeight;

	Animation::animationColumns = animationColumns;
};

Animation::Animation()
{
	Animation::animationImage = al_create_bitmap(1, 1);
	Animation::maxFrame = 1;
	Animation::frameDelay = 1;

	Animation::frameWidth = 1;
	Animation::frameHeight = 1;

	Animation::animationColumns = 1;
}

Animation::Animation(ALLEGRO_BITMAP* image, 
		float frameWidth, float frameHeight, 
		int maxFrame, int frameDelay,
		int animationColumns)
{
	Animation::animationImage = image;
	Animation::maxFrame = maxFrame;
	Animation::frameDelay = frameDelay;

	Animation::frameWidth = frameWidth;
	Animation::frameHeight = frameHeight;

	Animation::animationColumns = animationColumns;
};
/*void Animation::Render(RenderObject *parentGameObject)
{
	if(++frameCount >= frameDelay)
	{
		curFrame += animationDirection;
		if(curFrame >= maxFrame)
			curFrame = 0;
		else if (curFrame <= 0)
			curFrame = maxFrame;

		frameCount = 0;
	}

	int fx = (curFrame % animationColumns) * frameWidth;
	int fy = (curFrame / animationColumns) * frameHeight;

	if(animationImage) 
		al_draw_bitmap_region(animationImage, fx, fy, frameWidth, frameHeight,
		parentGameObject->GetX() + x - frameWidth / 2, parentGameObject->GetY() + y - frameHeight / 2, 0);
};*/