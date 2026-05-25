#pragma once
#include "GameObject.h"

class Animation;
class b2Body;

class CollectorRocket : public GameObject
{
protected:
	//Rocket Configuration
	float thrustAngle;
	float desiredAngle;

	//Body Moved by the rocket
	GameObject* attachedBody;

	//Organize this
	b2Body *attacher;

	//Fire Animation
	RenderObject *fireRender;

	//Rocket on
	bool on;
public:
	CollectorRocket::CollectorRocket();

	void Init(float x, float y, float angle, GameObject *attachedObject, b2World *world, Game *game);

	void SetDesiredAngle(float newDesiredAngle){ desiredAngle = newDesiredAngle; };

	//Set rocket on
	void StartRocket() { on = true; };

	void virtual Update(float cameraXOffset = 0, float cameraYOffset = 0, bool mouseIntegration = false, float mouseX = 0, float mouseY = 0);
	void virtual Render(float globalCeroX = 0, float globalCeroY = 0, float globalCeroRot = 0, float globalCeroXScale = 1, float globalCeroYScale = 1);
};