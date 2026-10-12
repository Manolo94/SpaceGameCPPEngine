#pragma once

#include "GameObject.h"

class Player : public GameObject
{
public:
	//All functions related to the player specific Class
	//Propulsor
	virtual void StartPropulsor() = 0;
	virtual void StopPropulsor() = 0;

	//Pulse
	virtual void StartPulse() = 0;
	virtual void StopPulse() = 0;
};