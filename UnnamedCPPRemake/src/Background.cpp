#include <allegro5/allegro5.h>
#include "Animation.h"
#include "Background.h"
#include "Camera.h"
#include "Globals.h"

Background::Background(Animation *animation, int x, int y, float rotation, float width, float height, float relativeSpeed, Camera *masterCamera, bool tiled)
{
	Background::masterCamera = masterCamera;

	Background::animation = animation;
	renderable = true;

	Background::x = x;
	Background::y = y;
	Background::rotation = rotation;
	Background::rotationCenterX = 0;
	Background::rotationCenterY = 0;
		
	Background::width = width;
	Background::height = height;

	Background::curFrame = 0;
	Background::frameCount = 0;
	Background::animationDirection = 1;

	Background::scaleX = (RenderObject::width)/(animation->frameWidth);
	Background::scaleY = (RenderObject::height)/(animation->frameHeight);

	Background::relativeSpeed = relativeSpeed;
	Background::tiled = tiled;

	masterCamera->AddBackground(this);
};

//Background Portion Draw
void Background::drawBackgroundPortion(float startCutPointX, float endCutPointX, float startCutPointY, float endCutPointY, float drawPointX, float drawPointY)
{
	al_draw_tinted_scaled_rotated_bitmap_region(animation->animationImage, startCutPointX, startCutPointY, endCutPointX - startCutPointX, endCutPointY - startCutPointY,
												al_map_rgb(255, 255, 255), 0, 0, drawPointX, drawPointY, scaleX, scaleY,
												rotation, 0);
};

void Background::Render(bool debug, bool frozenFrame)
{
	if (tiled)
	{
		Background::x = -int(masterCamera->focusX*relativeSpeed) % int(Background::GetWidth());
		if (x < 0) x = Background::GetWidth() + x;
		Background::y = -int(masterCamera->focusY*relativeSpeed) % int(Background::GetHeight());
		if (y < 0) y = Background::GetHeight() + y;
	}
	else
	{
		Background::x = -masterCamera->focusY*mToPx*relativeSpeed;
		Background::y = -masterCamera->focusY*mToPx*relativeSpeed;
	};

	//Cutting Settings
	float backgroundTileXCount = (WIDTH/scaleX)/(animation->frameWidth);
	float backgroundTileYCount = (HEIGHT/scaleY)/(animation->frameHeight);

	int backgroundTileXPiece = int(WIDTH/scaleX)%int(animation->frameWidth);
	int backgroundTileYPiece = int(HEIGHT/scaleY)%int(animation->frameHeight);

	if(backgroundTileXPiece < 0) backgroundTileXPiece += animation->frameWidth;
	if(backgroundTileYPiece < 0) backgroundTileYPiece += animation->frameHeight;

	if(x > backgroundTileXPiece) 
	{
		backgroundTileXCount -= 1;
	}
	if(y > backgroundTileYPiece) 
	{
		backgroundTileYCount -= 1;
	};
	//TILES
	if(tiled)
	{
		//CENTER Background Tiles
		for(int i = 0; i < backgroundTileXCount; i++)
		{
			for(int j = 0; j < backgroundTileYCount; j++)
			{
				drawBackgroundPortion(0, width/scaleX, 0, height/scaleY,x+width*i, y+height*j);
			};
		};

		//TOP and BOTTOM Background Cutted Tiles
		for(int i = 0; i < backgroundTileXCount; i++)
		{
			//TOP
			drawBackgroundPortion(0, width/scaleX, (height-y)/scaleY, height/scaleY, x+width*i, 0);
			//BOTTOM
			drawBackgroundPortion(0, width/scaleX, 0, backgroundTileYPiece - y/scaleY, x+width*i, HEIGHT - backgroundTileYPiece*scaleY + y);
		};

		//LEFT and RIGHT Background Cutted Tiles
		for(int i = 0; i < backgroundTileYCount; i++)
		{
			//LEFT
			drawBackgroundPortion((width-x)/scaleX, width/scaleX, 0, height/scaleY, 0, y+height*i);
			//RIGHT
			drawBackgroundPortion(0, backgroundTileXPiece - x/scaleX, 0, height/scaleY, WIDTH - backgroundTileXPiece*scaleX + x, y+height*i);
		};
	};
	//CORNERS
	//TOP LEFT CORNER
	drawBackgroundPortion((width-x)/scaleX, width/scaleX, (height-y)/scaleY, height/scaleY, 0, 0);
	if(!tiled) return;
	//BOTTOM LEFT CORNER
	drawBackgroundPortion((width-x)/scaleX, width/scaleX, 0, backgroundTileYPiece - y/scaleY, 0, HEIGHT - backgroundTileYPiece*scaleY + y);
	//TOP RIGHT CORNER
	drawBackgroundPortion(0, backgroundTileXPiece - x/scaleX, (height-y)/scaleY, height/scaleY, WIDTH - backgroundTileXPiece*scaleX + x, 0);
	//BOTTOM RIGHT CORNER
	drawBackgroundPortion(0, backgroundTileXPiece - x/scaleX, 0, backgroundTileYPiece - y/scaleY, WIDTH - backgroundTileXPiece*scaleX + x, HEIGHT - backgroundTileYPiece*scaleY + y);
};