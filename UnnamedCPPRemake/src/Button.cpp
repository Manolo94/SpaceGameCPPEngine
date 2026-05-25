#include "Button.h"
#include "Animation.h"
#include "RenderObject.h"

Button::Button()
{
	GameObject::SetType(GAME_OBJECT_TYPES::BUTTON);
}

void Button::Init(Animation *anim, float x, float y, float width, float height, b2World *world, Game *game, bool UIButton)
{
	Button::UIButton = UIButton;

	GameObject::Init(anim, x, y, width, height, world, game, true);
	currentButtonState = DEFAULT;

	for (int i = 0; i < 3; i++)
	{
		FrameProperties[i][INIT_FRAME] = 0;
		FrameProperties[i][END_FRAME] = 0;
		FrameProperties[i][DIR_FRAME] = 0;
		FrameProperties[i][LOOP] = 0;
	}

	SimpleBox2D::dynamicRectangle(physicsObject, width, height, world);

	GameObject::GetPhysicsObject()->GetFixtureList()->SetSensor(true);
}

void Button::OnEnter(){ Button::SetButtonState(ON); };
void Button::OnExit(){ Button::SetButtonState(DEFAULT); };
void Button::OnClick() { Button::SetButtonState(CLICK); };

void Button::Update(float cameraXOffset, float cameraYOffset, bool mouseIntegration, float mouseX, float mouseY)
{
	if(Button::UIButton)
		GameObject::Update(0, 0, mouseIntegration, mouseX, mouseY);
	else
		GameObject::Update(cameraXOffset, cameraYOffset, mouseIntegration, mouseX, mouseY);

	if (GetRenderObject()->GetCurrentFrame() == FrameProperties[currentButtonState][END_FRAME]
		&& FrameProperties[currentButtonState][LOOP] == 0) GetRenderObject()->Pause();
	else if (GetRenderObject()->GetCurrentFrame() == FrameProperties[currentButtonState][END_FRAME]
		&& FrameProperties[currentButtonState][LOOP] != 0) GetRenderObject()->ToFrame(FrameProperties[currentButtonState][INIT_FRAME],
		FrameProperties[currentButtonState][DIR_FRAME]);
}

void Button::Render()
{
	GameObject::Render();
};

//Animation methods
//DirFrame: 1 - forward, -1 - backwards, 0 - frozen frame
//Loop: 0 - no loop, else loop

void Button::SetAnimation_DEFAULT(int initFrame, int endFrame, int dirFrame, int loop)
{
	FrameProperties[DEFAULT][INIT_FRAME] = initFrame;
	FrameProperties[DEFAULT][END_FRAME] = endFrame;
	FrameProperties[DEFAULT][DIR_FRAME] = dirFrame;
	FrameProperties[DEFAULT][LOOP] = loop;
}

void Button::SetAnimation_ON(int initFrame, int endFrame, int dirFrame, int loop)
{
	FrameProperties[ON][INIT_FRAME] = initFrame;
	FrameProperties[ON][END_FRAME] = endFrame;
	FrameProperties[ON][DIR_FRAME] = dirFrame;
	FrameProperties[ON][LOOP] = loop;
}

void Button::SetAnimation_CLICK(int initFrame, int endFrame, int dirFrame, int loop)
{
	FrameProperties[CLICK][INIT_FRAME] = initFrame;
	FrameProperties[CLICK][END_FRAME] = endFrame;
	FrameProperties[CLICK][DIR_FRAME] = dirFrame;
	FrameProperties[CLICK][LOOP] = loop;
}

void Button::SetButtonState(int newState)
{
	currentButtonState = newState;
	GetRenderObject()->SetAnimDir(FrameProperties[currentButtonState][DIR_FRAME]);
}
Button::BUTTON_STATES Button::GetButtonState()
{
	return BUTTON_STATES(currentButtonState);
};