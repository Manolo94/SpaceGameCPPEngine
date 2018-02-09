#pragma once

#include "GameObject.h"

class Button : public GameObject
{
private:
	int currentButtonState;

	bool UIButton;
	
	//Animation handlers
	int FrameProperties[3][4];
public:
	Button();

	//Initialize
	virtual void Init(Animation *anim, float x, float y, float width, float height, b2World *world, Game *game, bool UIButton = false);

	//Update
	void Update(float cameraXOffset = 0, float cameraYOffset = 0, bool mouseIntegration = false, float mouseX = 0, float mouseY = 0);

	//Render
	virtual void Render();

	//Passing this to the derived classes
	//When the mouse is on the button. Called every update
	void OnEnter();
	//When the mouse clicks on the button
	void OnClick();
	//When the mouse is not on the button. Called every Update
	void OnExit();

	//ANIMATION

	//Constants
	enum BUTTON_STATES{ DEFAULT, ON, CLICK };
	enum BUTTON_STATES_PROPERTIES{ INIT_FRAME, END_FRAME, DIR_FRAME, LOOP };

	//Methods
	void SetAnimation_DEFAULT(int initFrame, int endFrame, int dirFrame, int loop);
	void SetAnimation_ON(int initFrame, int endFrame, int dirFrame, int loop);
	void SetAnimation_CLICK(int initFrame, int endFrame, int dirFrame, int loop);

	//Button State
	void SetButtonState(int newState);
	BUTTON_STATES GetButtonState();
};