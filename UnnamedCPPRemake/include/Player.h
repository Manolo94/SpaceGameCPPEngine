#pragma once

#include "GameObject.h"

class Player : public GameObject
{
public:
	//All functions related to the player specific Class
	//Propulsor
	virtual void Player::StartPropulsor() = 0;
	virtual void Player::StopPropulsor() = 0;

	//Pulse
	virtual void Player::StartPulse() = 0;
	virtual void Player::StopPulse() = 0;
};