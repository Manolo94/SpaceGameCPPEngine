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
	void Collector_Default::StartState();
	void Collector_Default::UpdateState();
	void Collector_Default::EndState();
	void Collector_Default::RightDown();
	void Collector_Default::RightUp();
	void Collector_Default::OnEnterObject(GameObject *objectTouched);
};

//Mouse State. Put Rocket in collector mode
class Collector_PutRocket : public MouseState
{
public:
	int cooldownCounter;

	Collector_PutRocket(MouseObject* mouseObject);
	void Collector_PutRocket::StartState();
	void Collector_PutRocket::UpdateState();
	void Collector_PutRocket::CustomBackRender(float x, float y);
	void Collector_PutRocket::LeftDown();
	void Collector_PutRocket::OnEnterObject(GameObject *objectTouched);
	void Collector_PutRocket::OnExitObject();
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
	void Collector_RocketAngle::StartState();
	void Collector_RocketAngle::UpdateState();
	void Collector_RocketAngle::LeftUp();

	void Collector_RocketAngle::CustomFrontRender(float x, float y);
};

