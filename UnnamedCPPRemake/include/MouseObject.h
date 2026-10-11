#pragma once

class Game;
class Animation;
class GameObject;
class RenderObject;
class b2World;
class b2Body;
class MouseState;

class MouseObject
{
private:
	//Master Game
	Game* masterGame;

	//Screen position (pixels) of the cursor
	float screenX;
	float screenY;
	//View position (unzoomed pixels): what GameObjects and mouse states work with
	float x;
	float y;

	//MouseState
	MouseState* mouseState;

	//Distance and angle to player
	float DistToPlayer = 0;
	float AngleToPlayer = 0;

	//Cursor RenderObject
	RenderObject* mouseCursor;

	//Touching object
	bool touching;
	GameObject* touchObject;
public:
	MouseObject(Game* game);

	//Getters
	float GetX() {return x;};
	float GetY() { return y; };
	float GetScreenX() { return screenX; };
	float GetScreenY() { return screenY; };
	float GetDistToPlayer(){ return DistToPlayer; };
	float GetAngleToPlayer(){ return AngleToPlayer; };
	RenderObject* GetCursor(){ return mouseCursor; };
	Game* GetMasterGame(){ return masterGame; };

	//State Machine
	//Change MouseState
	void ChangeMouseState(MouseState* newMouseState);
	
	//Returns the object touched by the mouse cursor
	GameObject *GetTouchingObject() 
	{ 
		if (touching) return touchObject; 
		else return 0;
	};

	//Set distance from mouse to player
	void SetDistToPlayer(float dist) { DistToPlayer = dist; };
	//Set angle from mouse to player
	void SetAngleToPlayer(float angle) { AngleToPlayer = angle; };

	//Change the appereance of the cursor
	void ChangeCursor(Animation* cursorSkin);

	//Set the object touched by the cursor
	void SetTouchingObject(GameObject *touch)
	{
		touching = true;
		touchObject = touch; 
	};

	//Reset Touching
	void ResetTouch()
	{
		touching = false;
	};

	//MouseEvents
	void RightClickDown();
	void RightClickUp();
	void LeftClickDown();
	void LeftClickUp();

	//Update
	void Update(float mouseX, float mouseY, float offSetX, float offSetY);

	//Render
	void Render();
};