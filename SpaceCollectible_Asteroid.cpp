#include "SpaceCollectible_Asteroid.h"
#include "Game.h"

SpaceCollectible_Asteroid::SpaceCollectible_Asteroid(Animation *animation, float initX, float initY, float radius, float initVelX, float initVelY, b2World *world, Game *game)
{
	SpaceCollectible::Init(animation, initX, initY, 2*radius, 2*radius, world, game);

	SimpleBox2D::dynamicCircle(physicsObject, radius, world);
	SimpleBox2D::SetLinearVelocity(physicsObject, initVelX, initVelY);
};