#pragma once

#include "SpaceCollectible.h"

class Game;

class SpaceCollectible_Asteroid : public SpaceCollectible
{
private:

public:
	SpaceCollectible_Asteroid(Animation *animation, float initX, float initY, float radius, float initVelX, float initVelY, b2World *world, Game *game);
};