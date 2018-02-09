#pragma once

#include "GameObject.h"

class SpaceCollectible : public GameObject
{
private:

public:
	SpaceCollectible();

	void Init(Animation *animation, float initX, float initY, float width, float height, b2World *world, Game *game);

	void virtual Update(float cameraXOffset = 0, float cameraYOffset = 0);
	
	virtual void Render(float globalCeroX = 0, float globalCeroY = 0, float globalCeroRot = 0, float globalCeroXScale = 1, float globalCeroYScale = 1);
};