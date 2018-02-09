#pragma once

#include "SpaceCollectible.h"

class SpaceCollectible_Column : public SpaceCollectible
{
public:
	SpaceCollectible_Column(Animation *animation, float initX, float initY, float width, float initVelX, float initVelY, b2World *world, Game *game);
};