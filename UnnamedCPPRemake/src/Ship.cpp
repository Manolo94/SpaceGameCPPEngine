#include "Ship.h"
#include "ShipModule.h"
#include "Game.h"

Ship::Ship(ShipModule *root, b2World *world, Game *game)
{
	b2Body *rootBody = root->GetPhysicsObject();
	b2Vec2 position = rootBody->GetPosition();

	//The ship has no picture of its own, its modules render relative to it
	GameObject::Init(NULL, position.x*mToPx, position.y*mToPx, SHIP_CELL_SIZE*mToPx, SHIP_CELL_SIZE*mToPx, world, game, MAIN_LAYER);
	SetType(GAME_OBJECT_TYPES::SHIP);

	physicsObject->SetTransform(position, rootBody->GetAngle());
	physicsObject->SetAngularVelocity(rootBody->GetAngularVelocity());
	physicsObject->SetAngularDamping(0.6f);

	Attach(root, 0, 0, 0);
};

bool Ship::HasCell(int x, int y)
{
	for (size_t i = 0; i < modules.size(); i++)
		if (modules[i]->GetCellX() == x && modules[i]->GetCellY() == y) return true;
	return false;
};

bool Ship::CanAttachAt(int x, int y)
{
	if (HasCell(x, y)) return false;
	return HasCell(x + 1, y) || HasCell(x - 1, y) || HasCell(x, y + 1) || HasCell(x, y - 1);
};

void Ship::Attach(ShipModule *module, int cellX, int cellY, int quarterTurns)
{
	b2Body *moduleBody = module->GetPhysicsObject();

	//Keep the combined linear momentum
	float shipMass = modules.empty() ? 0 : physicsObject->GetMass();
	float moduleMass = moduleBody->GetMass();
	b2Vec2 momentum = shipMass*physicsObject->GetLinearVelocity() + moduleMass*moduleBody->GetLinearVelocity();

	b2PolygonShape box;
	box.SetAsBox(SHIP_CELL_SIZE / 2, SHIP_CELL_SIZE / 2, b2Vec2(cellX*SHIP_CELL_SIZE, cellY*SHIP_CELL_SIZE), 0);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = module->GetDensity();
	fixtureDef.friction = 0.3f;
	fixtureDef.userData = module;
	physicsObject->CreateFixture(&fixtureDef);

	physicsObject->SetLinearVelocity((1.0f / (shipMass + moduleMass))*momentum);

	modules.push_back(module);
	module->AttachTo(this, cellX, cellY, quarterTurns);
};

int Ship::GetBindings(ShipModule *thruster)
{
	if (thruster->IsManuallyWired()) return thruster->GetManualBindings();

	//Ship-local frame: forward is -Y, positive torque turns clockwise on screen
	b2Vec2 direction = thruster->GetThrustDirectionLocal();
	b2Vec2 lever = thruster->GetCellCenterLocal() - physicsObject->GetLocalCenter();
	float torque = lever.x*direction.y - lever.y*direction.x;
	//A thruster counts as a turning thruster when its lever arm is at least a quarter cell
	float turnThreshold = 0.25f*SHIP_CELL_SIZE;

	int bindings = 0;
	if (direction.y < -0.5f) bindings |= SHIP_CONTROL_FORWARD;
	if (direction.y > 0.5f) bindings |= SHIP_CONTROL_BACKWARD;
	if (torque > turnThreshold) bindings |= SHIP_CONTROL_TURN_RIGHT;
	if (torque < -turnThreshold) bindings |= SHIP_CONTROL_TURN_LEFT;
	//Only a sideways thruster in line with the centre of mass strafes cleanly, the rest are left for manual wiring
	if (fabs(torque) <= turnThreshold && direction.x < -0.5f) bindings |= SHIP_CONTROL_STRAFE_LEFT;
	if (fabs(torque) <= turnThreshold && direction.x > 0.5f) bindings |= SHIP_CONTROL_STRAFE_RIGHT;
	return bindings;
};

int Ship::Fire(int controls, bool dryRun)
{
	int fired = 0;
	for (size_t i = 0; i < modules.size(); i++)
	{
		ShipModule *module = modules[i];
		if (module->GetModuleType() != MODULE_TYPE_THRUSTER) continue;

		module->UpdateTestBurst(*masterGame->GetMainCamera()->GetTimeStep());
		bool fire = (GetBindings(module) & controls) != 0;
		if (fire && !dryRun)
		{
			b2Vec2 direction = module->GetThrustDirectionLocal();
			physicsObject->ApplyForce(physicsObject->GetWorldVector(SHIP_THRUSTER_FORCE*direction),
				physicsObject->GetWorldPoint(module->GetCellCenterLocal()), true);
			fired++;
		}
		module->SetFiring(fire);
	}
	return fired;
};

void Ship::Update(float cameraXOffset, float cameraYOffset, bool mouseIntegration, float mouseX, float mouseY)
{
	GameObject::Update(cameraXOffset, cameraYOffset, mouseIntegration, mouseX, mouseY);

	//GameObject only tests the first fixture against the mouse
	b2Vec2 mouseWorld(mouseX / mToPx + cameraXOffset, mouseY / mToPx + cameraYOffset);
	for (b2Fixture *fixture = physicsObject->GetFixtureList(); fixture != NULL; fixture = fixture->GetNext())
	{
		if (fixture->TestPoint(mouseWorld))
		{
			masterGame->GetMouse()->SetTouchingObject(this);
			break;
		}
	}
};
