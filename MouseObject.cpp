#include "MouseObject.h"
#include "Game.h"
#include "GameObject.h"
#include "MouseState.h"

class MouseDefaultCursor : public MouseState{};

MouseObject::MouseObject(Game* game)
{
	masterGame = game;

	x = 0;
	y = 0;

	MouseObject::mouseCursor = new RenderObject();

	MouseObject::mouseState = new MouseDefaultCursor();
	mouseCursor->Init(game->GetResourcesManager()->retrieveResource_ANIM(CURSOR), 0, 0, 0, 50, 50, game, NO_RENDER);
};

//Change MouseState
void MouseObject::ChangeMouseState(MouseState* newMouseState)
{
	MouseObject::mouseState->EndState();
	MouseObject::mouseState = newMouseState;
	MouseObject::mouseState->StartState();
	MouseObject::ChangeCursor(mouseState->GetCursor());
};

//Change Cursor
void MouseObject::ChangeCursor(Animation* cursorSkin)
{
	MouseObject::mouseCursor->ChangeAnimation(cursorSkin, FRONT_LAYER);
};

//Events
void MouseObject::LeftClickDown() 
{ 
	//Call click for buttons
	if (GetTouchingObject() != NULL) GetTouchingObject()->OnClick();
	mouseState->LeftDown();
};
void MouseObject::LeftClickUp() { mouseState->LeftUp(); };
void MouseObject::RightClickDown() { mouseState->RightDown(); };
void MouseObject::RightClickUp() { mouseState->RightUp(); };

//Update Mouse
void MouseObject::Update(float mouseX, float mouseY, float offSetX, float offSetY)
{
	MouseObject::x = mouseX;
	MouseObject::y = mouseY;

	//Give mouse angle with the player
	SetAngleToPlayer(GeneralFunctions::getAngle(masterGame->GetPlayer()->GetRenderObject()->GetX(),
		masterGame->GetPlayer()->GetRenderObject()->GetY(), mouseX, mouseY));

	//Give mouse distance with the player
	SetDistToPlayer(GeneralFunctions::getDist(masterGame->GetPlayer()->GetRenderObject()->GetX(),
		masterGame->GetPlayer()->GetRenderObject()->GetY(), mouseX, mouseY));

	float border = 2;

	if (x > WIDTH - border) x = WIDTH - border;
	if (x < border) x = border;
	if (y > HEIGHT - border) y = HEIGHT - border;
	if (y < border) y = border;

	MouseObject::mouseCursor->SetX(x);
	MouseObject::mouseCursor->SetY(y);

	if (GetTouchingObject() != NULL) mouseState->OnEnterObject(GetTouchingObject());
	else if (GetTouchingObject() == NULL) mouseState->OnExitObject();

	mouseState->UpdateState();
};

//Render Mouse
void MouseObject::Render()
{
	mouseState->CustomBackRender(x, y);
	mouseCursor->Render();
	mouseState->CustomFrontRender(x, y);
};