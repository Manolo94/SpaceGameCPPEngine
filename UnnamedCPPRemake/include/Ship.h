#pragma once

#include <vector>
#include "GameObject.h"

class ShipModule;

//Ship controls as bit flags, so a thruster can be wired to any combination of them
enum SHIP_CONTROLS
{
	SHIP_CONTROL_FORWARD = 1,      //W
	SHIP_CONTROL_BACKWARD = 2,     //S
	SHIP_CONTROL_TURN_LEFT = 4,    //A
	SHIP_CONTROL_TURN_RIGHT = 8,   //D
	SHIP_CONTROL_STRAFE_LEFT = 16, //Q
	SHIP_CONTROL_STRAFE_RIGHT = 32 //E
};
const int SHIP_CONTROL_COUNT = 6;

//A rigid ship assembled from ShipModules. All modules are fixtures of one body laid on a grid of
//SHIP_CELL_SIZE cells, so thrusters apply force at their own position and the ship turns from physics.
class Ship : public GameObject
{
private:
	std::vector<ShipModule*> modules;

public:
	//Starts a ship from a loose module, which becomes cell (0, 0)
	Ship(ShipModule *root, b2World *world, Game *game);

	const std::vector<ShipModule*> &GetModules() { return modules; };
	bool HasCell(int x, int y);
	//A cell can take a module when it is free and touches an occupied cell
	bool CanAttachAt(int x, int y);
	//Attach a loose module; quarterTurns is its orientation relative to the ship
	void Attach(ShipModule *module, int cellX, int cellY, int quarterTurns);

	//Controls a thruster answers to: its manual wiring, or the ones its placement suggests
	int GetBindings(ShipModule *thruster);
	//Fire the thrusters wired to any of the held controls (SHIP_CONTROLS flags), returns how many fired.
	//A dry run only lights the plumes, used to preview the wiring.
	int Fire(int controls, bool dryRun = false);

	void virtual Update(float cameraXOffset = 0, float cameraYOffset = 0, bool mouseIntegration = false, float mouseX = 0, float mouseY = 0);
};
