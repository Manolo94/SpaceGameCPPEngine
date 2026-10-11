#pragma once

#include <list>
#include <vector>
#include <allegro5/allegro5.h>
#include <Box2D/Box2D.h>

#include "GameObject.h"
#include "Background.h"
#include "Indicator.h"

class Camera
{
private:
	//Parent game
	Game* masterGame;
	//Initialized ?
	bool init;

	//if moving to a point
	bool movingToPoint;
	//Time to get to it
	int movingTimeFrame;
	//Point to which the Camera is moving
	float moveX;
	float moveY;	

	//if following a GameObject
	bool follow;
	//velocity to follow
	float followVelocity;

	//GameObject being followed
	GameObject* followObject;

	//Zoom (1 = default). zoom eases towards targetZoom every update
	float zoom;
	float targetZoom;

	//Physics engine constants
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;
public:
	//Point inside the world focused by the camera (world units)
	float focusY;
	float focusX;

	//List of Layers
	std::vector <std::list<RenderObject *>*> RenderLayers;

	//GameObject List
	std::list<GameObject *> GameObjectList;
	
	//World at which the Camera is looking
	b2World* focusedWorld;

	//Default Constructor
	Camera();

	//Initializer
	void Init(float initFX, float initFY, b2World* world, Game* game);

	//Layers
	void AddBackground(RenderObject *background);
	void AddIndicator(RenderObject *indicator);
	void AddToLayer(RenderObject *renderObject, int layer);
	//Stop updating a GameObject (e.g. a module that became part of a ship). Not safe during Update()
	void RemoveGameObject(GameObject *gameObject) { GameObjectList.remove(gameObject); };
	
	//Movement
	void Move(float x,float y);
	void SetMovePoint(float x, float y, int timeFrame);
	void MoveTo(float x,float y, int timeFrame);
	void Follow(GameObject* followedObject, int timeFrame);
	void CeaseFollow() {
		follow = false;
		movingToPoint = false;
	};
	void SetFollowVelocity(float newFollowVelocity) {Camera::followVelocity = newFollowVelocity;};

	//World Properties Setters and geters
	float32* GetTimeStep() { return &timeStep; };
	int32* GetVelocityIterations() { return &velocityIterations; };
	int32* GetPositionIterations() { return &positionIterations; };

	void SetTimeStep(float32 newTimeStep) { timeStep = newTimeStep; };
	void SetVelocityIterations(float32 newVelIter) { velocityIterations = newVelIter; };
	void SetPositionIterations(float32 newPosIter) { positionIterations = newPosIter; };

	//Zoom
	void ZoomBy(float wheelSteps);
	float GetZoom() { return zoom; };
	//Transforms used while rendering: world layers are scaled around the screen centre, HUD is not
	void UseWorldTransform();
	void UseScreenTransform();
	//Convert a screen position (pixels) to view position (unzoomed pixels, what GameObjects use)
	float ScreenToViewX(float screenX) { return WIDTH / 2 + (screenX - WIDTH / 2) / zoom; };
	float ScreenToViewY(float screenY) { return HEIGHT / 2 + (screenY - HEIGHT / 2) / zoom; };
	//Convert a view position to world position (meters)
	float ViewToWorldX(float viewX) { return viewX / mToPx + focusX - WIDTH / 2 / mToPx; };
	float ViewToWorldY(float viewY) { return viewY / mToPx + focusY - HEIGHT / 2 / mToPx; };

	//Base Methods
	void Update(float mouseX, float mouseY);
	void Render();
};