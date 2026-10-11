#include "Astronaut.h"
#include "Game.h"

Astronaut::Astronaut(float initX, float initY, float initVelX, float initVelY, b2World *world, Game *game)
{
	masterGame = game;
	propulsion = false;
	pulse = false;

	ResourcesManager *resources = game->GetResourcesManager();
	Animation *idleAnim = resources->retrieveResource_ANIM(LUMA_IDLE);
	Animation *plumeAnim = resources->retrieveResource_ANIM(FX_LUMA_PLUME);
	Animation *toolAnim = resources->retrieveResource_ANIM(LUMA_TOOL);

	//LUMA-7 frames carry some padding around the robot, so the frame is a bit taller than the 1.7m body
	float bodyHeight = 1.8*mToPx;
	float bodyWidth = bodyHeight*idleAnim->frameWidth/idleAnim->frameHeight;

	//Backpack thruster plumes. Created before the body so they render behind it.
	float plumeHeight = bodyHeight*0.75;
	float plumeWidth = plumeHeight*plumeAnim->frameWidth/plumeAnim->frameHeight;
	for (int i = 0; i < 2; i++)
	{
		Astronaut::thrustPlumes[i] = new RenderObject();
		Astronaut::thrustPlumes[i]->Init(plumeAnim, bodyWidth*(i == 0 ? -0.12 : 0.12), bodyHeight*0.02, 0,
			plumeWidth, plumeHeight, game, MAIN_LAYER);
		//Anchor the plume at its nozzle end
		Astronaut::thrustPlumes[i]->SetRotationCenter(plumeAnim->frameWidth / 2, 0);
		Astronaut::thrustPlumes[i]->ToFrame(i*3);
		Astronaut::thrustPlumes[i]->Hide();
	}

	Astronaut::Init(idleAnim, initX, initY, bodyWidth, bodyHeight, world, game, MAIN_LAYER);
	Astronaut::propulsionPoint = new b2Vec2(0, -1);
	Astronaut::propulsionVector = new b2Vec2(5, 0);
	Astronaut::SetType(PLAYER_ASTRONAUT);

	for (int i = 0; i < 2; i++)
		Astronaut::thrustPlumes[i]->SetMasterRenderObject(GameObject::GetRenderObject());

	//Collector emitter, pivots at its back end and aims at the mouse
	float toolWidth = bodyWidth*0.6;
	Astronaut::toolAnimation = new RenderObject();
	Astronaut::toolAnimation->Init(toolAnim, 0, -bodyHeight*0.12, 0,
		toolWidth, toolWidth*toolAnim->frameHeight/toolAnim->frameWidth, game, MAIN_LAYER);
	Astronaut::toolAnimation->SetRotationCenter(0, toolAnim->frameHeight / 2);
	Astronaut::toolAnimation->ToFrame(0, 0);
	Astronaut::toolAnimation->Hide();
	Astronaut::toolAnimation->SetMasterRenderObject(GameObject::GetRenderObject());

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
	if (propulsion) return;
	propulsion = true;
	renderObject->SetAnimation(masterGame->GetResourcesManager()->retrieveResource_ANIM(LUMA_THRUST));
	for (int i = 0; i < 2; i++)
		Astronaut::thrustPlumes[i]->Unhide();
};
void Astronaut::StopPropulsor() 
{
	propulsion = false;
	renderObject->SetAnimation(masterGame->GetResourcesManager()->retrieveResource_ANIM(LUMA_IDLE));
	for (int i = 0; i < 2; i++)
		Astronaut::thrustPlumes[i]->Hide();
};

//Pulse
void Astronaut::StartPulse()
{
	pulse = true;
	Astronaut::toolAnimation->ToFrame(1, 1);
};
void Astronaut::StopPulse()
{
	pulse = false;
	Astronaut::toolAnimation->ToFrame(0, 0);
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

	//Collector tool animation conditions
	if (masterGame->GetMouse()->GetTouchingObject() != NULL 
		&& masterGame->GetMouse()->GetTouchingObject()->GetType() == GAME_OBJECT_TYPES::SPACE_COLLECTIBLE)
	{
		toolAnimation->Unhide();
		toolAnimation->SetRotation(masterGame->GetMouse()->GetAngleToPlayer()+3.14);
	}
	else toolAnimation->Hide();

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