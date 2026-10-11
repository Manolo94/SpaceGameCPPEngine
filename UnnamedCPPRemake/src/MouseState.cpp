#include "MouseState.h"
#include "MouseObject.h"
#include "Game.h"
#include "GameObject.h"

//****************************************************************
//						CURSOR STATES
//****************************************************************

//Propulsor
Collector_Default::Collector_Default(MouseObject* mouseObject)
{
	Collector_Default::Init(mouseObject);
};

void Collector_Default::StartState()
{
	MouseState::cursor = masterMouseObject->GetMasterGame()->
		GetResourcesManager()->retrieveResource_ANIM(CURSOR);
};

void Collector_Default::UpdateState()
{
	Collector_Default::masterMouseObject->GetCursor()->SetRotation(masterMouseObject->GetAngleToPlayer());
};

void Collector_Default::EndState()
{
	Collector_Default::RightUp();
};

void Collector_Default::RightDown()
{
	if (masterMouseObject->GetMasterGame()->GetMouse()->GetTouchingObject() == NULL &&
		masterMouseObject->GetMasterGame()->GetGameStat(PLAYER_FUEL) > 0 && masterMouseObject->GetMasterGame()->GetGameState() == GAME_STATES::RESOURCE_COLLECTION)
		masterMouseObject->GetMasterGame()->GetPlayer()->StartPropulsor();
	else if (masterMouseObject->GetMasterGame()->GetMouse()->GetTouchingObject() != 0)
		masterMouseObject->GetMasterGame()->GetPlayer()->StartPulse();
};

void Collector_Default::RightUp()
{
	masterMouseObject->GetMasterGame()->GetPlayer()->StopPropulsor();
	masterMouseObject->GetMasterGame()->GetPlayer()->StopPulse();
};

void Collector_Default::LeftDown()
{
	Game *game = masterMouseObject->GetMasterGame();
	if (game->GetGameState() != GAME_STATES::RESOURCE_COLLECTION) return;
	if (game->GetShipBuilder()->StartDrag(masterMouseObject->GetTouchingObject()))
		masterMouseObject->ChangeMouseState(new Builder_Drag(masterMouseObject));
};

void Collector_Default::OnEnterObject(GameObject *objectTouched)
{
	if (objectTouched->GetType() == GAME_OBJECT_TYPES::SPACE_COLLECTIBLE && masterMouseObject->GetDistToPlayer() <= 4 * mToPx)
	{
		MouseState* startPuttingRocket = new Collector_PutRocket(masterMouseObject);
		masterMouseObject->ChangeMouseState(startPuttingRocket);
	};
};
//***************************************************************************************************

//Builder Drag
Builder_Drag::Builder_Drag(MouseObject* mouseObject)
{
	Builder_Drag::Init(mouseObject);
};

void Builder_Drag::StartState()
{
	masterMouseObject->GetCursor()->SetRotation(0);
	cursor = masterMouseObject->GetMasterGame()->GetResourcesManager()->retrieveResource_ANIM(CURSOR);
};

void Builder_Drag::EndState()
{
	masterMouseObject->GetMasterGame()->GetShipBuilder()->EndDrag();
};

void Builder_Drag::LeftUp()
{
	masterMouseObject->ChangeMouseState(new Collector_Default(masterMouseObject));
};

//***************************************************************************************************
//Collect Put Rocket
Collector_PutRocket::Collector_PutRocket(MouseObject* mouseObject)
{
	//Cooldown to cero
	Collector_PutRocket::cooldownCounter = 0;

	//Initialize
	Collector_PutRocket::Init(mouseObject);
};
void Collector_PutRocket::StartState()
{
	masterMouseObject->GetCursor()->SetRotation(0);
	Collector_PutRocket::cursor = masterMouseObject->GetMasterGame()->
		GetResourcesManager()->retrieveResource_ANIM(CURSOR_ROCKET);
};
void Collector_PutRocket::UpdateState()
{
};
void Collector_PutRocket::CustomBackRender(float x, float y)
{
	if (Collector_PutRocket::cooldownCounter <= 100) Collector_PutRocket::cooldownCounter += 1;
	al_draw_filled_circle(x, y, 25*cooldownCounter/100, al_map_rgba(0, 30, 255, 0.5));
};
void Collector_PutRocket::LeftDown()
{
	if (masterMouseObject->GetTouchingObject() != NULL && cooldownCounter >= 95)
	{
		if (masterMouseObject->GetTouchingObject()->GetType() == GAME_OBJECT_TYPES::SPACE_COLLECTIBLE)
		{
			Collector_RocketAngle *selectAngle = new Collector_RocketAngle(masterMouseObject);
			masterMouseObject->ChangeMouseState(selectAngle);
		};
	cooldownCounter = 0;
	};
};
void Collector_PutRocket::OnEnterObject(GameObject *objectTouched)
{
	if (objectTouched->GetType() != GAME_OBJECT_TYPES::SPACE_COLLECTIBLE || masterMouseObject->GetDistToPlayer() > 4 * mToPx)
	{
		MouseState* stopPuttingObject = new Collector_Default(masterMouseObject);
		masterMouseObject->ChangeMouseState(stopPuttingObject);
	};
	//printf("DIS %f = %f\n", masterMouseObject->GetDistToPlayer(), 4 * mToPx);
};
void Collector_PutRocket::OnExitObject()
{
	MouseState* stopPuttingObject = new Collector_Default(masterMouseObject);
	masterMouseObject->ChangeMouseState(stopPuttingObject);
};

//****************************************************************************************************

//Collect Rocket Angle
Collector_RocketAngle::Collector_RocketAngle(MouseObject* mouseObject)
{
	Collector_RocketAngle::Init(mouseObject);
};

void Collector_RocketAngle::StartState()
{
	//Empty cursor
	cursor = masterMouseObject->GetMasterGame()->
		GetResourcesManager()->retrieveResource_ANIM(NULL_ANIM);

	//Getting the aimed resource
	aimedResource = masterMouseObject->GetTouchingObject();

	rocketAngle = 0;

	initialX = masterMouseObject->GetX();
	initialY = masterMouseObject->GetY();

	//Put the rocket without angle
	Collector_RocketAngle::rocket = new CollectorRocket();
	RaycastHandler* raycatty = new RaycastHandler();
	raycatty->SetTargetFixture(aimedResource->GetPhysicsObject()->GetFixtureList());
	masterMouseObject->GetMasterGame()->GetMainCamera()->focusedWorld->
		RayCast(raycatty, masterMouseObject->GetMasterGame()->GetPlayer()->GetPhysicsObject()->GetPosition(),
		aimedResource->GetPhysicsObject()->GetPosition());

	rocket->Init(raycatty->GetPointX()*mToPx, raycatty->GetPointY()*mToPx, raycatty->GetNormalAngle(), aimedResource,
		masterMouseObject->GetMasterGame()->GetMainCamera()->focusedWorld, masterMouseObject->GetMasterGame());
};

void Collector_RocketAngle::UpdateState()
{
	rocketAngle = GeneralFunctions::getAngle(masterMouseObject->GetX(), masterMouseObject->GetY(), initialX, initialY);
};

void Collector_RocketAngle::CustomFrontRender(float x, float y)
{
	/*3D Rendering ????? LOL
	initialX - 10 * sin(rocketAngle), initialY - 10 * cos(rocketAngle)
	initialX + 10 * cos(rocketAngle), initialY + 10 * sin(rocketAngle)
	initialX + 10 * sin(rocketAngle), initialY + 10 * cos(rocketAngle)
							
	those coordinates draw a triangle in 3d, that changes its orientation with the rocketAngle*/

	float drawX = aimedResource->GetRenderObject()->GetX();
	float drawY = aimedResource->GetRenderObject()->GetY();

	float triangleSize = 30;
	float distBetTriangles = 60;

	for (int i = 0; i < 10; i++)
	{
		float modInitX = drawX + i * distBetTriangles * cos(rocketAngle);
		float modInitY = drawY + i * distBetTriangles * sin(rocketAngle);

		al_draw_filled_triangle(modInitX + triangleSize * sin(rocketAngle), modInitY - triangleSize * cos(rocketAngle),
								modInitX + triangleSize * cos(rocketAngle), modInitY + triangleSize * sin(rocketAngle),
								modInitX - triangleSize * sin(rocketAngle), modInitY + triangleSize * cos(rocketAngle),
								al_map_rgb(0, 255, 200));
	}
}

void Collector_RocketAngle::LeftUp()
{
	//Finish passing the desiredAngle
	rocket->SetDesiredAngle(rocketAngle);
	rocket->StartRocket();

	Collector_Default *returnToDefault = new Collector_Default(masterMouseObject);
	masterMouseObject->ChangeMouseState(returnToDefault);
}