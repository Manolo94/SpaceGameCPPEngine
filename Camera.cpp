#include "Camera.h"
#include "Globals.h"
#include "Game.h"
#include <list>

Camera::Camera()
{
	Camera::init = false;

	Camera::focusX = 0;
	Camera::focusY = 0;

	Camera::movingToPoint = false;
	Camera::movingTimeFrame = 1;

	Camera::moveX = 0;
	Camera::moveY = 0;

	Camera::follow = false;
	Camera::followVelocity = .05;
};

void Camera::Init(float initFX, float initFY, b2World* world, Game* game)
{
	Camera::focusX = initFX;
	Camera::focusY = initFY;

	Camera::movingToPoint = false;
	Camera::movingTimeFrame = 1;

	Camera::moveX = 0;
	Camera::moveY = 0;

	Camera::follow = false;
	Camera::followVelocity = .05;

	Camera::focusedWorld = world;
	Camera::masterGame = game;

	//Set layers
	for (int i = 0; i < LAYERS_TOTAL; i++)
	{
		RenderLayers.push_back(new std::list<RenderObject *>);
	}

	init = true;
};
void Camera::AddBackground(RenderObject *background)
{
	Camera::RenderLayers[BACKGROUND_LAYER]->push_back(background);
};
void Camera::AddIndicator(RenderObject *indicator)
{
	Camera::RenderLayers[FRONT_LAYER]->push_back(indicator);
};

void Camera::AddToLayer(RenderObject *renderObject, const int layer)
{
	Camera::RenderLayers[layer]->push_back(renderObject);
}

void Camera::Move(float x,float y)
{
	Camera::focusX += x;
	Camera::focusY += y;
};
void Camera::SetMovePoint(float x, float y, int timeFrame)
{
	movingToPoint = true;
	movingTimeFrame = timeFrame;

	Camera::moveX = x;
	Camera::moveY = y;
};
void Camera::MoveTo(float x,float y, int timeFrame)
{
	Camera::CeaseFollow();

	Camera::SetMovePoint(x, y, timeFrame);
};
void Camera::Follow(GameObject* followedObject, int timeFrame)
{
	Camera::followObject = followedObject;
	Camera::follow = true;

	Camera::movingTimeFrame = timeFrame;
};
void Camera::Update(float mouseX, float mouseY)
{
	//DEBUG
	if (masterGame->CheckKeyPressed(ALLEGRO_KEY_RIGHT)) Camera::Move(1, 0);
	if (masterGame->CheckKeyPressed(ALLEGRO_KEY_LEFT)) Camera::Move(-1, 0);
	if (masterGame->CheckKeyPressed(ALLEGRO_KEY_UP)) Camera::Move(0, -1);
	if (masterGame->CheckKeyPressed(ALLEGRO_KEY_DOWN)) Camera::Move(0, 1);
	//DEBUG

	if(movingToPoint)
	{
		if(movingTimeFrame >= 1)
		{
			Camera::Move((Camera::moveX - Camera::focusX)/Camera::movingTimeFrame, (Camera::moveY - Camera::focusY)/Camera::movingTimeFrame);
			movingTimeFrame--;
		};
	};
	if(follow)
	{
		Camera::SetMovePoint(followObject->GetPhysicsObject()->GetPosition().x,
			followObject->GetPhysicsObject()->GetPosition().y, 1/Camera::followVelocity);
	};

	//FIRST... UPDATE MOUSE OBJECT... 
	//reset touching Object
	masterGame->GetMouse()->ResetTouch();

	//UPDATE GameObjects
	std::list<GameObject *>::iterator iterUpdate;
	for (iterUpdate = GameObjectList.begin(); iterUpdate != GameObjectList.end(); iterUpdate++)
	{
		(*iterUpdate)->Update(Camera::focusX - WIDTH / 2 / mToPx, Camera::focusY - HEIGHT / 2 / mToPx, true, mouseX, mouseY);
	}
};
void Camera::Render()
{
	//RENDER FOR ALL LAYERS
	for (int i = 1; i < LAYERS_TOTAL; i++)
	{
		std::list<RenderObject *>::iterator iterRender;
		for (iterRender = RenderLayers[i]->begin(); iterRender != RenderLayers[i]->end(); iterRender++)
		{
			(*iterRender)->Render();
		}
	};
	// LAST... RENDER MOUSE CURSOR.. Mouse is above any layer, NO EXCEPTIONS...
	masterGame->GetMouse()->Render();
};