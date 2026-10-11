#pragma once

#include <vector>
#include "GameObject.h"

class ShipModule;

//Ship thruster commands
struct ShipControls
{
	bool forward;
	bool backward;
	bool turnLeft;
	bool turnRight;
};

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

	//Fire the thrusters matching the controls, returns how many fired
	int Fire(const ShipControls &controls);

	void virtual Update(float cameraXOffset = 0, float cameraYOffset = 0, bool mouseIntegration = false, float mouseX = 0, float mouseY = 0);
};
