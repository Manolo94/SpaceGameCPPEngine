#pragma once

#include "GameObject.h"

class Ship;

enum SHIP_MODULE_TYPES {MODULE_TYPE_HULL, MODULE_TYPE_THRUSTER};

//A single ship building block. It floats freely with its own body until it is attached to a Ship,
//then it becomes one fixture of the ship's body and only keeps its RenderObject.
class ShipModule : public GameObject
{
private:
	SHIP_MODULE_TYPES moduleType;

	//Ship this module belongs to (NULL while loose)
	Ship *ship;
	//Cell inside the ship grid and orientation in quarter turns
	int cellX;
	int cellY;
	int quarterTurns;

	//Exhaust plume (thrusters only, created when attached)
	RenderObject *plume;
	//Seconds left of a test burst shown after rewiring
	float testBurst;

	//Thruster wiring (SHIP_CONTROLS flags). Until the player rewires it, the ship picks the controls from its placement.
	bool manuallyWired;
	int bindings;

public:
	//x, y in world pixels, angle in radians
	ShipModule(SHIP_MODULE_TYPES type, float x, float y, float angle, b2World *world, Game *game);

	SHIP_MODULE_TYPES GetModuleType() { return moduleType; };
	bool IsAttached() { return ship != NULL; };
	Ship *GetShip() { return ship; };
	int GetCellX() { return cellX; };
	int GetCellY() { return cellY; };
	int GetQuarterTurns() { return quarterTurns; };
	float GetDensity() { return moduleType == MODULE_TYPE_THRUSTER ? 1.5f : 1.0f; };

	//Thrust direction and nozzle position in ship-local meters
	b2Vec2 GetThrustDirectionLocal();
	b2Vec2 GetCellCenterLocal();

	//Called by Ship: gives up the module's own body and renders relative to the ship
	void AttachTo(Ship *newShip, int newCellX, int newCellY, int newQuarterTurns);

	//Show/hide the exhaust plume. A pending test burst keeps it lit.
	void SetFiring(bool firing);
	//Flash the plume for a moment without pushing the ship
	void TestBurst() { testBurst = 0.3f; };
	void UpdateTestBurst(float dt) { if (testBurst > 0) testBurst -= dt; };

	bool IsManuallyWired() { return manuallyWired; };
	int GetManualBindings() { return bindings; };
	void SetManualBindings(int newBindings) { manuallyWired = true; bindings = newBindings; };
};
