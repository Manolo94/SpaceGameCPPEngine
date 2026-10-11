#pragma once

#include <list>
#include <Box2D/Box2D.h>
#include "ShipModule.h"

class Game;
class Ship;
class GameObject;

//Where a dragged module would snap if released now
struct ShipSnap
{
	bool valid;
	//Target: an existing ship, or a loose module that would start a new ship
	Ship *ship;
	ShipModule *looseModule;
	int cellX;
	int cellY;
	int quarterTurns;
};

//Spawns modules, drags them with LUMA-7's tractor beam, snaps them into ships and lets the player pilot.
//
//Controls: 1/2 spawn hull/thruster at the cursor, left-drag a module or ship to move it,
//Q/E rotate the dragged module, release next to a module to attach, F boards/leaves the nearest
//ship, W/S/A/D fire its thrusters while aboard.
class ShipBuilder
{
private:
	Game *game;
	b2World *world;

	std::list<ShipModule*> looseModules;
	std::list<Ship*> ships;

	//Tractor beam drag
	b2Body *ground;
	b2MouseJoint *dragJoint;
	GameObject *dragged;
	float dragAngle;
	ShipSnap snap;

	//Piloting
	Ship *piloted;
	b2Joint *pilotJoint;

	b2Vec2 MouseWorld();
	b2Vec2 PlayerWorld();
	ShipSnap FindSnap(ShipModule *module);
	//Distance from a point to the nearest module of a ship
	float DistanceToShip(Ship *ship, b2Vec2 point);
	void ToggleBoarding();

public:
	ShipBuilder(Game *game, b2World *world);

	ShipModule *SpawnModule(SHIP_MODULE_TYPES type);

	//Tractor beam. StartDrag returns false when the object can't be grabbed (wrong type, out of reach)
	bool StartDrag(GameObject *object);
	void EndDrag();
	bool IsDragging() { return dragJoint != NULL; };
	bool IsPiloting() { return piloted != NULL; };

	void OnKeyUp(int keycode);
	void Update();
	//Tractor beam and snap preview, drawn in view coordinates with the world transform active
	void RenderOverlay();
};
