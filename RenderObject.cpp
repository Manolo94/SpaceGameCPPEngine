#include "RenderObject.h"
#include "GameObject.h"
#include "Game.h"
#include "Animation.h"
#include <math.h>

void RenderObject::Init(Animation *animation, int x, int y, float rotation, float width, float height, Game *masterGame, int renderLayer, bool fixed)
{
	RenderObject::animation = animation;
	renderable = true;

	RenderObject::x = x;
	RenderObject::globalX = x;
	RenderObject::y = y;
	RenderObject::globalY = y;
	RenderObject::rotation = rotation;

	RenderObject::width = width;
	RenderObject::height = height;

	RenderObject::curFrame = 0;
	RenderObject::frameCount = 0;
	RenderObject::animationDirection = 1;

	if (animation != NULL)
	{
		RenderObject::scaleX = (RenderObject::width) / (animation->frameWidth);
		RenderObject::scaleY = (RenderObject::height) / (animation->frameHeight);
		RenderObject::rotationCenterX = animation->frameWidth / 2;
		RenderObject::rotationCenterY = animation->frameHeight / 2;
	}
	else
	{
		RenderObject::scaleX = 1.0;
		RenderObject::scaleY = 1.0;
		RenderObject::rotationCenterX = width / 2;
		RenderObject::rotationCenterY = height / 2;
	}

	RenderObject::fixed = fixed;

	RenderObject::masterRenderObject = NULL;

	RenderObject::masterGame = masterGame;
	RenderObject::masterGame->GetMainCamera()->AddToLayer(this, renderLayer);
};

void RenderObject::SetMasterRenderObject(RenderObject *masterRenderObject)
{
	RenderObject::masterRenderObject = masterRenderObject;
}

void RenderObject::Play(int animDir)
{
	RenderObject::animationDirection = animDir;
};
void RenderObject::Pause()
{
	RenderObject::animationDirection = 0;
};
void RenderObject::Stop()
{
	RenderObject::animationDirection = 0;
	RenderObject::curFrame = 0;
};
/*Changes the current frame. You can also manually change the direction of the animation with animDir:
1 - forward
0 - frozen frame
-1 - backwards
*/
void RenderObject::ToFrame(int frame, int animDir)
{
	RenderObject::animationDirection = animDir;

	RenderObject::curFrame = frame;
};

//Set Width
void RenderObject::SetWidth(float newWidth) 
{ 
	width = newWidth;
	//Update ScaleX after
	RenderObject::scaleX = (RenderObject::width) / (animation->frameWidth);
};
//Set Height
void RenderObject::SetHeight(float newHeight)
{
	height = newHeight;
	//Update ScaleY after
	RenderObject::scaleY = (RenderObject::height) / (animation->frameHeight);
};
//Set ScaleX
void RenderObject::SetScaleX(float newScaleX)
{
	scaleX = newScaleX;
	//Update Width after
	width = scaleX * animation->frameWidth;
}
//Set ScaleY
void RenderObject::SetScaleY(float newScaleY)
{
	scaleY = newScaleY;
	//Update Height after
	height = scaleY * animation->frameHeight;
}

void RenderObject::Update(float x, float y, float rot, float scaleX, float scaleY)
{
	RenderObject::x = x;
	RenderObject::y = y;
	RenderObject::rotation = rot;
	RenderObject::scaleX = scaleX;
	RenderObject::scaleY = scaleY;
}

void RenderObject::Render(bool debug, bool frozenFrame)
{
	if (masterRenderObject != NULL)
	{
		FinalRender(masterRenderObject->GetX(), masterRenderObject->GetY(), masterRenderObject->GetRotation(), masterRenderObject->GetScaleX(),
			masterRenderObject->GetScaleY(), debug, frozenFrame);
	}
	else FinalRender();
}

void RenderObject::FinalRender(float globalCeroX, float globalCeroY, float globalCeroRot, float globalCeroXScale, float globalCeroYScale, bool debug, bool frozenFrame)
{
	if (fixed)
	{
		globalCeroX = 0;
		globalCeroY = 0;
		globalCeroRot = 0;
		globalCeroXScale = 1.0f;
		globalCeroYScale = 1.0f;
	}
	if(renderable && animation != NULL)
	{
		if(!frozenFrame)
		{
			if(++frameCount >= animation->frameDelay)
			{
				curFrame += RenderObject::animationDirection;
				if(curFrame >= animation->maxFrame)
					curFrame = 0;
				else if (curFrame < 0)
					curFrame = animation->maxFrame;

				frameCount = 0;
			};
		};
		float fx = (curFrame % (animation->animationColumns)) * (animation->frameWidth);
		float fy = (curFrame / (animation->animationColumns)) * (animation->frameHeight);

		globalX = globalCeroX + x*cos(globalCeroRot) - y*sin(globalCeroRot);
		globalY = globalCeroY + x*sin(globalCeroRot) + y*cos(globalCeroRot);

		if(!debug)
		{
			al_draw_tinted_scaled_rotated_bitmap_region(animation->animationImage, fx, fy, animation->frameWidth, animation->frameHeight,
				al_map_rgb(255, 255, 255), rotationCenterX, rotationCenterY, globalX, globalY, scaleX*globalCeroXScale, scaleY*globalCeroYScale,
			globalCeroRot + rotation, 0);
		}
		else
		{
			al_draw_tinted_scaled_rotated_bitmap_region(animation->animationImage, fx, fy, animation->frameWidth, animation->frameHeight,
				al_map_rgb(100, 255, 255), (animation->frameWidth/2), (animation->frameHeight/2), globalX, globalY, scaleX*globalCeroXScale, scaleY*globalCeroYScale,
			globalCeroRot + rotation, 0);
		}
	};
};

//Debug
void RenderObject::DrawRotationCenter()
{
	al_draw_circle(globalX, globalY, 3, al_map_rgb(255, 0, 0), 4);
};