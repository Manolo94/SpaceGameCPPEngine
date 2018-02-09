#include "SpaceCollectible.h"

SpaceCollectible::SpaceCollectible()
{
	SpaceCollectible::SetType(GAME_OBJECT_TYPES::SPACE_COLLECTIBLE);
};
void SpaceCollectible::Init(Animation *animation, float initX, float initY, float width, float height, b2World *world, Game *game)
{
	GameObject::Init(animation, initX, initY, width, height, world, game, MAIN_LAYER);
};

void SpaceCollectible::Update(float cameraXOffset, float cameraYOffset)
{
	GameObject::Update(cameraXOffset, cameraYOffset);
}

void SpaceCollectible::Render(float globalCeroX, float globalCeroY, float globalCeroRot, float globalCeroXScale, float globalCeroYScale)
{
	GameObject::Render();
};