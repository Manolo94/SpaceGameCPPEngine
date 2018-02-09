#include "Astronaut.h"
#include "Game.h"

Astronaut::Astronaut(float initX, float initY, float initVelX, float initVelY, b2World *world, Game *game)
{
	masterGame = game;

	Astronaut::Init(game->GetResourcesManager()->retrieveResource_ANIM(ASTRONAUT_BODY), initX, initY, 
		1.7*mToPx*30/74, 1.7*mToPx, world, game, MAIN_LAYER);
	Astronaut::propulsionPoint = new b2Vec2(0, -1);
	Astronaut::propulsionVector = new b2Vec2(5, 0);
	Astronaut::SetType(PLAYER_ASTRONAUT);

	Astronaut::fireAnimation = new RenderObject();
	Astronaut::fireAnimation->Init(game->GetResourcesManager()->retrieveResource_ANIM(FX_THRUST_FIRE), 
		renderObject->GetWidth()*(-0.15), renderObject->GetHeight()*0.4, 0, renderObject->GetWidth()*23/30, 
		renderObject->GetHeight()*50/74, game, MAIN_LAYER);
	Astronaut::fireAnimation->Hide();
	Astronaut::fireAnimation->SetMasterRenderObject(GameObject::GetRenderObject());

	Astronaut::pistolAnimation = new RenderObject();
	Astronaut::pistolAnimation->Init(game->GetResourcesManager()->retrieveResource_ANIM(ASTRONAUT_PISTOL), 0, -5, 0, 
		game->GetResourcesManager()->retrieveResource_ANIM(ASTRONAUT_PISTOL)->frameWidth*3/4, 
		game->GetResourcesManager()->retrieveResource_ANIM(ASTRONAUT_PISTOL)->frameHeight, game, MAIN_LAYER);
	Astronaut::pistolAnimation->SetRotationCenter(0, pistolAnimation->GetHeight() / 2);
	Astronaut::pistolAnimation->SetMasterRenderObject(GameObject::GetRenderObject());

	//PHYSICS OBJECT SETUP

	SimpleBox2D::dynamicRectangle(physicsObject, 0.4*mToPx, 1.5*mToPx, world);
	//Astronaut's head
	b2Body *astroHead = SimpleBox2D::preparePhysicsObject(initX, initY - 20, world);
	SimpleBox2D::dynamicCircle(astroHead, 8, world);			
			
	//Astronaut's bag
	b2Body *astroBag = SimpleBox2D::preparePhysicsObject(initX - 5, initY + 1, world);
	SimpleBox2D::dynamicRectangle(astroBag, 0.3*mToPx, 0.4*mToPx, world);

	//Putting Everything together... (head rotational center)
	//...body-head
	b2WeldJointDef *bodyHeadWeldDef = new b2WeldJointDef();
	bodyHeadWeldDef->Initialize(physicsObject, astroHead, astroHead->GetWorldCenter());
			
	joint = world->CreateJoint(bodyHeadWeldDef);
			
	//...body-bag
	b2WeldJointDef *bodyBagWeldDef = new b2WeldJointDef();
	bodyBagWeldDef->Initialize(physicsObject, astroBag, astroBag->GetWorldCenter());
			
	world->CreateJoint(bodyBagWeldDef);

	physicsObject->SetLinearVelocity(b2Vec2(initVelX, initVelY));
};

//Propulsor
void Astronaut::StartPropulsor() 
{
	propulsion = true;
	Astronaut::fireAnimation->Unhide();
};
void Astronaut::StopPropulsor() 
{
	propulsion = false;
	Astronaut::fireAnimation->Hide();
};

//Pulse
void Astronaut::StartPulse()
{
	pulse = true;
};
void Astronaut::StopPulse()
{
	pulse = false;
};

void Astronaut::Update(float cameraXOffset, float cameraYOffset, bool mouseIntegration, float mouseX, float mouseY)
{
	//***************************GAME OBJECT UPDATE*******************************
	GameObject::Update(cameraXOffset, cameraYOffset, mouseIntegration, mouseX, mouseY);

	//***********************ASTRONAUT PLAYER PROCEDURES**********************

	//Can't do anything without fuel...
	if (masterGame->GetGameStat(PLAYER_FUEL) <= 0)
	{
		Astronaut::StopPropulsor();
		Astronaut::StopPulse();
	};

	//Declare global distance to the center
	masterGame->SetGameStat(GeneralFunctions::getDist(Astronaut::GetPhysicsObject()->GetPosition().x,
														Astronaut::GetPhysicsObject()->GetPosition().y,
														0, 0), PLAYER_DISTANCE_TO_CENTER);

	//Declare global angle with the center
	masterGame->SetGameStat(GeneralFunctions::getAngle(0,0,
													Astronaut::GetPhysicsObject()->GetPosition().x,
													Astronaut::GetPhysicsObject()->GetPosition().y), 
													PLAYER_ANGLE_TO_CENTER);

	//Fire animation conditions
	if(propulsion)
	{		
		Astronaut::propulsionVector->Set(-cos(masterGame->GetMouse()->GetAngleToPlayer())*8, 
			-sin(masterGame->GetMouse()->GetAngleToPlayer())*8);
		Astronaut::GetPhysicsObject()->ApplyForce(*(Astronaut::propulsionVector), 
			Astronaut::GetPhysicsObject()->GetWorldPoint(b2Vec2(0, 0)), true);
		masterGame->SetGameStat(masterGame->GetGameStat(PLAYER_FUEL)-0.001, PLAYER_FUEL);
	}

	//Pistol animation conditions
	if (masterGame->GetMouse()->GetTouchingObject() != NULL 
		&& masterGame->GetMouse()->GetTouchingObject()->GetType() == GAME_OBJECT_TYPES::SPACE_COLLECTIBLE)
	{
		pistolAnimation->Unhide();
		pistolAnimation->SetRotation(masterGame->GetMouse()->GetAngleToPlayer()+3.14);
	}
	else pistolAnimation->Hide();

	//Pulse animation
	if (pulse && masterGame->GetMouse()->GetTouchingObject() != NULL)
	{
		masterGame->GetMouse()->GetTouchingObject()->GetPhysicsObject()->
			ApplyForceToCenter(b2Vec2(-cos(masterGame->GetMouse()->GetAngleToPlayer()) * 20, -sin(masterGame->GetMouse()->GetAngleToPlayer()) * 20), true);
	};

	//Death
	if (joint != NULL)
	{
		//printf("Reaction Force %f \n", joint->GetReactionForce(60).LengthSquared());
		if (joint->GetReactionForce(60).LengthSquared() > 10000)
		{
			masterGame->GetMainCamera()->focusedWorld->DestroyJoint(joint);
			joint = NULL;
		}
	};
};

void Astronaut::Render(float globalCeroX, float globalCeroY, float globalCeroRot, float globalCeroXScale, float globalCeroYScale)
{	
	//Pulse
	if (pulse) al_draw_filled_circle(renderObject->GetX() + -cos(masterGame->GetMouse()->GetAngleToPlayer()) * 50, 
		renderObject->GetY() + -sin(masterGame->GetMouse()->GetAngleToPlayer()) * 50, 30, al_map_rgb(200, 200, 255));
	
	//Astronaut's body
	GameObject::Render(globalCeroX, globalCeroY, globalCeroRot, globalCeroXScale, globalCeroYScale);\
};