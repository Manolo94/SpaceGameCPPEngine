#include "CollectorRocket.h"
#include "Game.h"
#include "Animation.h"

CollectorRocket::CollectorRocket()
{
	CollectorRocket::SetType(GAME_OBJECT_TYPES::ROCKET);
};

void CollectorRocket::Init(float x, float y, float angle, GameObject *attachedObject, b2World *world, Game *game)
{
	masterGame = game;
	
	GameObject::Init(masterGame->GetResourcesManager()->retrieveResource_ANIM(ROCKET_ENGINE), x, y, 55, 35, world, game, MAIN_LAYER);

	fireRender = new RenderObject();
	fireRender->Init(masterGame->GetResourcesManager()->retrieveResource_ANIM(FX_THRUST_FIRE), 45, 0, -3.14/2, 35, 50, game, MAIN_LAYER);
	fireRender->SetMasterRenderObject(GameObject::GetRenderObject());

	CollectorRocket::desiredAngle = 0;
	CollectorRocket::thrustAngle = angle;

	CollectorRocket::attachedBody = attachedObject;

	SimpleBox2D::dynamicCircle(physicsObject, 10, world);

	b2Body* engine = SimpleBox2D::preparePhysicsObject(x+10*cos(thrustAngle), y+10*sin(thrustAngle), world);
	SimpleBox2D::dynamicRectangle(engine, 45, 25, world);
	engine->SetTransform(engine->GetPosition(), thrustAngle);

	b2WeldJointDef* attacherToBody = new b2WeldJointDef();
	attacherToBody->Initialize(physicsObject, attachedBody->GetPhysicsObject(), physicsObject->GetWorldCenter());

	b2RevoluteJointDef* attacherToEngine = new b2RevoluteJointDef();
	attacherToEngine->Initialize(engine, physicsObject, physicsObject->GetWorldCenter());

	attacherToEngine->motorSpeed = desiredAngle - thrustAngle;

	world->CreateJoint(attacherToBody);
	world->CreateJoint(attacherToEngine);

	attacher = physicsObject;
	physicsObject = engine;

	on = false;
};

void CollectorRocket::Update(float cameraXOffset, float cameraYOffset, bool mouseIntegration, float mouseX, float mouseY)
{
	GameObject::Update(cameraXOffset, cameraYOffset, mouseIntegration, mouseX, mouseY);
	//Desired Angle to the origin (0,0)
	desiredAngle = GeneralFunctions::getAngle(CollectorRocket::GetPhysicsObject()->GetPosition().x,
		CollectorRocket::GetPhysicsObject()->GetPosition().y, 0, 0)-3.14;
	if (on)
	{
		CollectorRocket::thrustAngle = (CollectorRocket::GetPhysicsObject()->GetAngle() + 3.14);

		if (thrustAngle <= desiredAngle + 3.14 / 10 && thrustAngle >= desiredAngle - 3.14 / 10)
			attacher->ApplyForceToCenter(b2Vec2(20 * cos(desiredAngle), 20 * sin(desiredAngle)), false);

		attachedBody->GetPhysicsObject()->ApplyTorque(20 * (desiredAngle - thrustAngle), false);
		attachedBody->GetPhysicsObject()->SetAngularDamping(1 / (abs(desiredAngle - thrustAngle)));
		//printf("%f \n", thrustAngle);
		fireRender->Unhide();
	}
	else fireRender->Hide();
};
void CollectorRocket::Render(float globalCeroX, float globalCeroY, float globalCeroRot, float globalCeroXScale, float globalCeroYScale)
{
	GameObject::Render(globalCeroX, globalCeroY, globalCeroRot, globalCeroXScale, globalCeroYScale);
};