#include "SpaceCollectible_Column.h"

SpaceCollectible_Column::SpaceCollectible_Column(Animation *animation, float initX, float initY, float width, float initVelX, float initVelY, b2World *world, Game *game)
{
	SpaceCollectible::Init(animation, initX, initY, width, 0.5*mToPx, world, game);

	SimpleBox2D::dynamicRectangle(physicsObject, width, 0.5*mToPx, world);
	SimpleBox2D::SetLinearVelocity(physicsObject, initVelX, initVelY);
};