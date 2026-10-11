#pragma once

#include "Animation.h"
#include "State.h"

class MouseObject;
class GameObject;
class CollectorRocket;

class MouseState : public State
{
protected:
	Animation *cursor;

	MouseObject* masterMouseObject;
public:
	void Init(MouseObject* mouseObject)
	{
		masterMouseObject = mouseObject;
	};

	//Left Click
	virtual void LeftDown(){};
	virtual void LeftUp(){};

	//Right Click
	virtual void RightDown(){};
	virtual void RightUp(){};

	//Touch Object Event
	virtual void OnEnterObject(GameObject* objectTouched){};
	virtual void OnExitObject(){};

	//Custom Render
	virtual void CustomBackRender(float x, float y){};
	virtual void CustomFrontRender(float x, float y){};
	
	//Getters
	Animation* GetCursor() { return cursor; };
};

//Mouse State. Default state in collector mode
class Collector_Default : public MouseState
{
public:
	Collector_Default(MouseObject* mouseObject);
	void StartState();
	void UpdateState();
	void EndState();
	void RightDown();
	void RightUp();
	void LeftDown();
	void OnEnterObject(GameObject *objectTouched);
};

//Mouse State. Dragging a ship module or ship with the tractor beam
class Builder_Drag : public MouseState
{
public:
	Builder_Drag(MouseObject* mouseObject);
	void StartState();
	void EndState();
	void LeftUp();
};

//Mouse State. Put Rocket in collector mode
class Collector_PutRocket : public MouseState
{
public:
	int cooldownCounter;

	Collector_PutRocket(MouseObject* mouseObject);
	void StartState();
	void UpdateState();
	void CustomBackRender(float x, float y);
	void LeftDown();
	void OnEnterObject(GameObject *objectTouched);
	void OnExitObject();
};

//Mouse State. Select Direction of the Rocket
class Collector_RocketAngle : public MouseState
{
private:
	float initialX;
	float initialY;

	float rocketAngle;
	CollectorRocket *rocket;
	GameObject *aimedResource;
public:
	Collector_RocketAngle(MouseObject* mouseObject);
	void StartState();
	void UpdateState();
	void LeftUp();

	void CustomFrontRender(float x, float y);
};

