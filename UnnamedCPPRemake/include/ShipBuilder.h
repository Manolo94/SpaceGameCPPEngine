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
//ship, W/S/A/D/Q/E fire its thrusters while aboard.
//
//Wiring: aboard, R opens the ship's wiring. Hover a thruster and tap W/A/S/D/Q/E to connect or
//disconnect it from that control (it test-fires when connected), C disconnects it from everything.
//Holding a control lights every thruster on it. Thrusters nobody rewired follow their placement.
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

	//Wiring mode, the thruster under the cursor while it is open
	bool wiring;
	ShipModule *hoveredThruster;

	b2Vec2 MouseWorld();
	b2Vec2 PlayerWorld();
	ShipSnap FindSnap(ShipModule *module);
	//Distance from a point to the nearest module of a ship
	float DistanceToShip(Ship *ship, b2Vec2 point);
	void ToggleBoarding();
	//Thruster of the piloted ship under a world point, NULL if none
	ShipModule *ThrusterAt(b2Vec2 point);
	//SHIP_CONTROLS flag for a key, 0 if the key isn't a ship control
	int ControlForKey(int keycode);
	//SHIP_CONTROLS flags of the held keys
	int HeldControls();
	//Thruster outlines, control badges and the patch lead while wiring
	void RenderWiring(float offsetX, float offsetY);

public:
	ShipBuilder(Game *game, b2World *world);

	ShipModule *SpawnModule(SHIP_MODULE_TYPES type);

	//Tractor beam. StartDrag returns false when the object can't be grabbed (wrong type, out of reach)
	bool StartDrag(GameObject *object);
	void EndDrag();
	bool IsDragging() { return dragJoint != NULL; };
	bool IsPiloting() { return piloted != NULL; };
	bool IsWiring() { return wiring; };

	void OnKeyUp(int keycode);
	void Update();
	//Tractor beam, snap preview and wiring labels, drawn in view coordinates with the world transform active
	void RenderOverlay();
};
