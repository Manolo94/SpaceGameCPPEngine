#pragma once

#include "Player.h"

class Game;

class Astronaut : public Player
{
private:
	//Game Holder
	Game* masterGame;

	RenderObject *thrustPlumes[2];
	RenderObject *toolAnimation;

	//Propulsor variables
	bool propulsion;
	b2Vec2* propulsionVector;
	b2Vec2* propulsionPoint;

	//Pistol pulse
	bool pulse;

	//Death
	//Head-Body Joint
	b2Joint* joint;
public:

	Astronaut(float initX, float initY, float initVelX, float initVelY, b2World *world, Game *game);

	//Propulsor Controllers
	virtual void StartPropulsor();
	virtual void StopPropulsor();

	//Pulse Controllers
	virtual void StartPulse();
	virtual void StopPulse();

	void virtual Update(float cameraXOffset, float cameraYOffset, bool mouseIntegration, float mouseX, float mouseY);
	
	void Render(float globalCeroX = 0, float globalCeroY = 0, float globalCeroRot = 0, float globalCeroXScale = 1, float globalCeroYScale = 1);
};