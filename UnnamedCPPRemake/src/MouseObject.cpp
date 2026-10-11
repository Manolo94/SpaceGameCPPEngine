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
	screenX = 0;
	screenY = 0;

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
	MouseObject::mouseCursor->SetAnimation(cursorSkin);
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
	float border = 2;
	if (mouseX > WIDTH - border) mouseX = WIDTH - border;
	if (mouseX < border) mouseX = border;
	if (mouseY > HEIGHT - border) mouseY = HEIGHT - border;
	if (mouseY < border) mouseY = border;

	MouseObject::screenX = mouseX;
	MouseObject::screenY = mouseY;
	MouseObject::x = masterGame->GetMainCamera()->ScreenToViewX(mouseX);
	MouseObject::y = masterGame->GetMainCamera()->ScreenToViewY(mouseY);

	//Give mouse angle with the player
	SetAngleToPlayer(GeneralFunctions::getAngle(masterGame->GetPlayer()->GetRenderObject()->GetX(),
		masterGame->GetPlayer()->GetRenderObject()->GetY(), x, y));
	//Give mouse distance with the player
	SetDistToPlayer(GeneralFunctions::getDist(masterGame->GetPlayer()->GetRenderObject()->GetX(),
		masterGame->GetPlayer()->GetRenderObject()->GetY(), x, y));

	MouseObject::mouseCursor->SetX(screenX);
	MouseObject::mouseCursor->SetY(screenY);

	if (GetTouchingObject() != NULL) mouseState->OnEnterObject(GetTouchingObject());
	else if (GetTouchingObject() == NULL) mouseState->OnExitObject();
	mouseState->UpdateState();
};
//Render Mouse
void MouseObject::Render()
{
	//Mouse state overlays live in the world, the cursor itself on screen
	masterGame->GetMainCamera()->UseWorldTransform();
	mouseState->CustomBackRender(x, y);
	masterGame->GetMainCamera()->UseScreenTransform();
	mouseCursor->Render();
	masterGame->GetMainCamera()->UseWorldTransform();
	mouseState->CustomFrontRender(x, y);
	masterGame->GetMainCamera()->UseScreenTransform();
};