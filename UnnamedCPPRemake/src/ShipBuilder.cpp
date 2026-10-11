#include "ShipBuilder.h"
#include "Ship.h"
#include "Game.h"

#include <allegro5/allegro_primitives.h>

ShipBuilder::ShipBuilder(Game *game, b2World *world)
{
	ShipBuilder::game = game;
	ShipBuilder::world = world;

	//Mouse joints need a static body to pull against
	b2BodyDef groundDef;
	ground = world->CreateBody(&groundDef);

	dragJoint = NULL;
	dragged = NULL;
	dragAngle = 0;
	snap.valid = false;

	piloted = NULL;
	pilotJoint = NULL;
};

b2Vec2 ShipBuilder::MouseWorld()
{
	Camera *camera = game->GetMainCamera();
	return b2Vec2(camera->ViewToWorldX(game->GetMouse()->GetX()), camera->ViewToWorldY(game->GetMouse()->GetY()));
};

b2Vec2 ShipBuilder::PlayerWorld()
{
	return game->GetPlayer()->GetPhysicsObject()->GetWorldCenter();
};

//=====================================================================
//SPAWNING (stand-in for harvesting modules from debris)
//=====================================================================
ShipModule *ShipBuilder::SpawnModule(SHIP_MODULE_TYPES type)
{
	b2Vec2 at = MouseWorld();
	ShipModule *module = new ShipModule(type, at.x*mToPx, at.y*mToPx, 0, world, game);
	looseModules.push_back(module);
	return module;
};

//=====================================================================
//TRACTOR BEAM
//=====================================================================
bool ShipBuilder::StartDrag(GameObject *object)
{
	if (dragJoint != NULL || object == NULL) return false;
	if (object->GetType() != GAME_OBJECT_TYPES::SHIP_MODULE && object->GetType() != GAME_OBJECT_TYPES::SHIP) return false;
	if ((MouseWorld() - PlayerWorld()).Length() > BUILD_REACH) return false;

	b2Body *body = object->GetPhysicsObject();
	b2MouseJointDef jointDef;
	jointDef.bodyA = ground;
	jointDef.bodyB = body;
	jointDef.target = MouseWorld();
	jointDef.maxForce = 60.0f*body->GetMass();
	jointDef.frequencyHz = 4.0f;
	jointDef.dampingRatio = 0.9f;
	dragJoint = (b2MouseJoint*)world->CreateJoint(&jointDef);
	body->SetAwake(true);

	dragged = object;
	//Modules are held square to the grid, Q/E turn them a quarter at a time
	dragAngle = b2_pi / 2 * round(body->GetAngle() / (b2_pi / 2));
	return true;
};

void ShipBuilder::EndDrag()
{
	if (dragJoint == NULL) return;
	world->DestroyJoint(dragJoint);
	dragJoint = NULL;

	if (snap.valid && dragged->GetType() == GAME_OBJECT_TYPES::SHIP_MODULE)
	{
		ShipModule *module = (ShipModule*)dragged;
		Ship *ship = snap.ship;
		if (ship == NULL)
		{
			//Two loose modules make a new ship
			ship = new Ship(snap.looseModule, world, game);
			looseModules.remove(snap.looseModule);
			ships.push_back(ship);
		}
		ship->Attach(module, snap.cellX, snap.cellY, snap.quarterTurns);
		looseModules.remove(module);
		//The module's body is gone, don't let anything use it through the mouse this frame
		game->GetMouse()->ResetTouch();
	}

	dragged = NULL;
	snap.valid = false;
};

ShipSnap ShipBuilder::FindSnap(ShipModule *module)
{
	ShipSnap best;
	best.valid = false;
	float bestError = SHIP_SNAP_DISTANCE;

	b2Vec2 center = module->GetPhysicsObject()->GetPosition();
	float angle = module->GetPhysicsObject()->GetAngle();

	auto consider = [&](b2Body *body, Ship *ship, ShipModule *loose)
	{
		b2Vec2 local = (1.0f / SHIP_CELL_SIZE)*body->GetLocalPoint(center);
		int cellX = (int)round(local.x);
		int cellY = (int)round(local.y);
		bool free = ship != NULL ? ship->CanAttachAt(cellX, cellY) : abs(cellX) + abs(cellY) == 1;
		float error = (local - b2Vec2((float)cellX, (float)cellY)).Length();
		if (!free || error >= bestError) return;

		bestError = error;
		best.valid = true;
		best.ship = ship;
		best.looseModule = loose;
		best.cellX = cellX;
		best.cellY = cellY;
		best.quarterTurns = (((int)round((angle - body->GetAngle()) / (b2_pi / 2))) % 4 + 4) % 4;
	};

	for (std::list<Ship*>::iterator it = ships.begin(); it != ships.end(); it++)
		consider((*it)->GetPhysicsObject(), *it, NULL);
	for (std::list<ShipModule*>::iterator it = looseModules.begin(); it != looseModules.end(); it++)
		if (*it != module) consider((*it)->GetPhysicsObject(), NULL, *it);

	return best;
};

//=====================================================================
//PILOTING
//=====================================================================
float ShipBuilder::DistanceToShip(Ship *ship, b2Vec2 point)
{
	float nearest = 1e9f;
	const std::vector<ShipModule*> &modules = ship->GetModules();
	for (size_t i = 0; i < modules.size(); i++)
	{
		b2Vec2 cell = ship->GetPhysicsObject()->GetWorldPoint(modules[i]->GetCellCenterLocal());
		float distance = (cell - point).Length() - SHIP_CELL_SIZE / 2;
		if (distance < nearest) nearest = distance;
	}
	return nearest;
};

void ShipBuilder::ToggleBoarding()
{
	if (piloted != NULL)
	{
		world->DestroyJoint(pilotJoint);
		pilotJoint = NULL;
		ShipControls none = {false, false, false, false};
		piloted->Fire(none);
		piloted = NULL;
		return;
	}

	Ship *nearest = NULL;
	float nearestDistance = BOARD_DISTANCE;
	for (std::list<Ship*>::iterator it = ships.begin(); it != ships.end(); it++)
	{
		float distance = DistanceToShip(*it, PlayerWorld());
		if (distance <= nearestDistance)
		{
			nearestDistance = distance;
			nearest = *it;
		}
	}
	if (nearest == NULL) return;

	//LUMA-7 pulls itself against the nearest module, then clamps on and rides along
	b2Body *playerBody = game->GetPlayer()->GetPhysicsObject();
	b2Body *shipBody = nearest->GetPhysicsObject();
	b2Vec2 playerCenter = playerBody->GetWorldCenter();
	b2Vec2 closestCell(0, 0);
	float closestDistance = 1e9f;
	const std::vector<ShipModule*> &modules = nearest->GetModules();
	for (size_t i = 0; i < modules.size(); i++)
	{
		b2Vec2 cell = shipBody->GetWorldPoint(modules[i]->GetCellCenterLocal());
		if ((cell - playerCenter).Length() < closestDistance)
		{
			closestDistance = (cell - playerCenter).Length();
			closestCell = cell;
		}
	}
	b2Vec2 away = playerCenter - closestCell;
	away.Normalize();
	b2Vec2 shift = closestCell + (SHIP_CELL_SIZE / 2 + 0.3f)*away - playerCenter;

	//Move the whole LUMA-7 rig (body plus the head and backpack welded to it) together
	std::vector<b2Body*> rig;
	rig.push_back(playerBody);
	for (b2JointEdge *edge = playerBody->GetJointList(); edge != NULL; edge = edge->next)
		if (edge->other != ground && edge->other != shipBody) rig.push_back(edge->other);
	for (size_t i = 0; i < rig.size(); i++)
	{
		rig[i]->SetTransform(rig[i]->GetPosition() + shift, rig[i]->GetAngle());
		rig[i]->SetLinearVelocity(shipBody->GetLinearVelocityFromWorldPoint(rig[i]->GetWorldCenter()));
		rig[i]->SetAngularVelocity(shipBody->GetAngularVelocity());
	}

	b2WeldJointDef weldDef;
	weldDef.Initialize(playerBody, nearest->GetPhysicsObject(), playerBody->GetWorldCenter());
	pilotJoint = world->CreateJoint(&weldDef);
	piloted = nearest;
};

//=====================================================================
//INPUT AND UPDATE
//=====================================================================
void ShipBuilder::OnKeyUp(int keycode)
{
	switch (keycode)
	{
	case ALLEGRO_KEY_1:
		SpawnModule(MODULE_TYPE_HULL);
		break;
	case ALLEGRO_KEY_2:
		SpawnModule(MODULE_TYPE_THRUSTER);
		break;
	case ALLEGRO_KEY_Q:
		dragAngle -= b2_pi / 2;
		break;
	case ALLEGRO_KEY_E:
		dragAngle += b2_pi / 2;
		break;
	case ALLEGRO_KEY_F:
		ToggleBoarding();
		break;
	}
};

void ShipBuilder::Update()
{
	if (dragJoint != NULL)
	{
		//The beam only reaches so far from LUMA-7
		b2Vec2 target = MouseWorld();
		b2Vec2 fromPlayer = target - PlayerWorld();
		if (fromPlayer.Length() > BUILD_REACH)
			target = PlayerWorld() + (BUILD_REACH / fromPlayer.Length())*fromPlayer;
		dragJoint->SetTarget(target);

		if (dragged->GetType() == GAME_OBJECT_TYPES::SHIP_MODULE)
		{
			//Steer the module towards its held orientation
			b2Body *body = dragged->GetPhysicsObject();
			float error = remainder(dragAngle - body->GetAngle(), 2 * b2_pi);
			body->SetAngularVelocity(8.0f*error);
			snap = FindSnap((ShipModule*)dragged);
		}
	}

	if (piloted != NULL)
	{
		ShipControls controls;
		controls.forward = game->CheckKeyPressed(ALLEGRO_KEY_W);
		controls.backward = game->CheckKeyPressed(ALLEGRO_KEY_S);
		controls.turnLeft = game->CheckKeyPressed(ALLEGRO_KEY_A);
		controls.turnRight = game->CheckKeyPressed(ALLEGRO_KEY_D);
		piloted->Fire(controls);
	}
};

void ShipBuilder::RenderOverlay()
{
	if (dragJoint == NULL) return;

	Camera *camera = game->GetMainCamera();
	float offsetX = camera->focusX - WIDTH / 2 / mToPx;
	float offsetY = camera->focusY - HEIGHT / 2 / mToPx;

	//Snap preview: a ghost of the module where it would attach
	if (snap.valid)
	{
		b2Body *body = snap.ship != NULL ? snap.ship->GetPhysicsObject() : snap.looseModule->GetPhysicsObject();
		b2Vec2 at = body->GetWorldPoint(b2Vec2(snap.cellX*SHIP_CELL_SIZE, snap.cellY*SHIP_CELL_SIZE));
		//Snaps only exist while dragging a module
		Animation *anim = game->GetResourcesManager()->retrieveResource_ANIM(
			((ShipModule*)dragged)->GetModuleType() == MODULE_TYPE_THRUSTER ? MODULE_THRUSTER : MODULE_HULL);
		float scale = SHIP_CELL_SIZE*mToPx / anim->frameWidth;
		al_draw_tinted_scaled_rotated_bitmap(anim->animationImage, al_map_rgba_f(0.15f, 0.45f, 0.45f, 0.5f),
			anim->frameWidth / 2, anim->frameHeight / 2, (at.x - offsetX)*mToPx, (at.y - offsetY)*mToPx,
			scale, scale, body->GetAngle() + snap.quarterTurns*b2_pi / 2, 0);
	}

	//Tractor beam from LUMA-7 to the grab point
	b2Vec2 from = PlayerWorld();
	b2Vec2 to = dragJoint->GetAnchorB();
	float x1 = (from.x - offsetX)*mToPx, y1 = (from.y - offsetY)*mToPx;
	float x2 = (to.x - offsetX)*mToPx, y2 = (to.y - offsetY)*mToPx;
	al_draw_line(x1, y1, x2, y2, al_map_rgba(0, 90, 110, 110), 8);
	al_draw_line(x1, y1, x2, y2, al_map_rgba(150, 255, 255, 255), 2);
	al_draw_filled_circle(x2, y2, 5, al_map_rgba(150, 255, 255, 255));
};
