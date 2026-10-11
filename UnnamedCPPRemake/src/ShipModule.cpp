#include "ShipModule.h"
#include "Ship.h"
#include "Game.h"

ShipModule::ShipModule(SHIP_MODULE_TYPES type, float x, float y, float angle, b2World *world, Game *game)
{
	moduleType = type;
	ship = NULL;
	plume = NULL;
	testBurst = 0;
	manuallyWired = false;
	bindings = 0;
	cellX = 0;
	cellY = 0;
	quarterTurns = 0;

	float sizePx = SHIP_CELL_SIZE*mToPx;
	Animation *anim = game->GetResourcesManager()->retrieveResource_ANIM(type == MODULE_TYPE_THRUSTER ? MODULE_THRUSTER : MODULE_HULL);
	GameObject::Init(anim, x, y, sizePx, sizePx, world, game, MAIN_LAYER);
	SetType(GAME_OBJECT_TYPES::SHIP_MODULE);

	b2PolygonShape box;
	box.SetAsBox(SHIP_CELL_SIZE / 2, SHIP_CELL_SIZE / 2);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = GetDensity();
	fixtureDef.friction = 0.3f;
	physicsObject->CreateFixture(&fixtureDef);
	physicsObject->SetTransform(physicsObject->GetPosition(), angle);
};

//Thrusters push away from their nozzle, which points down (+Y) in the sprite
b2Vec2 ShipModule::GetThrustDirectionLocal()
{
	float turn = quarterTurns*b2_pi / 2;
	return b2Vec2(sin(turn), -cos(turn));
};

b2Vec2 ShipModule::GetCellCenterLocal()
{
	return b2Vec2(cellX*SHIP_CELL_SIZE, cellY*SHIP_CELL_SIZE);
};

void ShipModule::AttachTo(Ship *newShip, int newCellX, int newCellY, int newQuarterTurns)
{
	ship = newShip;
	cellX = newCellX;
	cellY = newCellY;
	quarterTurns = newQuarterTurns;

	//The ship's body carries this module from now on
	masterGame->GetMainCamera()->RemoveGameObject(this);
	masterGame->GetMainCamera()->focusedWorld->DestroyBody(physicsObject);
	physicsObject = NULL;

	b2Vec2 center = GetCellCenterLocal();
	renderObject->SetMasterRenderObject(ship->GetRenderObject());
	renderObject->SetX(center.x*mToPx);
	renderObject->SetY(center.y*mToPx);
	renderObject->SetRotation(quarterTurns*b2_pi / 2);

	if (moduleType == MODULE_TYPE_THRUSTER)
	{
		//Plume hangs from the nozzle edge of the cell, pointing away from the thrust
		Animation *plumeAnim = masterGame->GetResourcesManager()->retrieveResource_ANIM(FX_LUMA_PLUME);
		b2Vec2 nozzle = center - 0.5f*SHIP_CELL_SIZE*GetThrustDirectionLocal();
		float plumeHeight = 1.1f*SHIP_CELL_SIZE*mToPx;
		plume = new RenderObject();
		plume->Init(plumeAnim, nozzle.x*mToPx, nozzle.y*mToPx, quarterTurns*b2_pi / 2,
			plumeHeight*plumeAnim->frameWidth/plumeAnim->frameHeight*1.6f, plumeHeight, masterGame, MAIN_LAYER);
		plume->SetRotationCenter(plumeAnim->frameWidth / 2, 0);
		plume->SetMasterRenderObject(ship->GetRenderObject());
		plume->Hide();
	}
};

void ShipModule::SetFiring(bool firing)
{
	if (plume == NULL) return;
	if (firing || testBurst > 0) plume->Unhide();
	else plume->Hide();
};
