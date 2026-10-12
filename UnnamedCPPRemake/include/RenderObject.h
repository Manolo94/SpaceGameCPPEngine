#pragma once

#include <allegro5/allegro.h>
#include <list>

class GameObject;
class Game;
class Animation;

class RenderObject
{
protected:
	//Game reference
	Game *masterGame;

	//master RenderObject Reference
	RenderObject *masterRenderObject;

	//General Properties
	float x;
	float globalX;
	float y;
	float globalY;
	float width;
	float height;
	float rotation;
	float rotationCenterX;
	float rotationCenterY;
	
	//ANIMATION
	//Store Object Animation
	Animation *animation;
	float scaleX;
	float scaleY;

	//Animation rendering variables
	int curFrame; //Animation Frame
	int frameCount; //Amount of real frames frames on each animation cycle
	int animationDirection; //Animation Direction
	
	bool renderable;
	//Fixed position, independent of camera position
	bool fixed;

public:

	//initializer
	void Init(Animation *animation, int x, int y, float rotation, float width, float height, Game *masterGame, int renderLayer, bool fixed = false);

	//Change Animation and Layer
	void ChangeAnimation(Animation *animation, int newLayer)
	{
		Init(animation, x, y, rotation, width, height, masterGame, newLayer);
	};

	//Swap Animation in place, keeping size, layer and master RenderObject
	void SetAnimation(Animation *animation);

	//Hide/Unhide RenderObject
	void Hide() {RenderObject::renderable = false;};
	void Unhide() {RenderObject::renderable = true;};

	//X, Y, Width, Heights, ScaleX, ScaleY Getters
	float GetX() {return x;};
	float GetY() {return y;};
	float GetRotation() {return rotation;};
	float GetWidth() {return width;};;
	float GetHeight() {return height;};
	float GetScaleX() {return scaleX;};
	float GetScaleY() {return scaleY;};

	//X, Y, Width, Heights, ScaleX, ScaleY Setters
	void SetX(float newX) {x = newX;};
	void SetY(float newY) {y = newY;};
	void SetRotation(float newRotation) {rotation = newRotation;};
	void SetRotationCenter(float newRotX, float newRotY)
	{
		rotationCenterX = newRotX;
		rotationCenterY = newRotY;
	};
	void SetWidth(float newWidth);
	void SetHeight(float newHeight);
	void SetScaleX(float newScaleX);
	void SetScaleY(float newScaleY);

	//Set master RenderObject
	void SetMasterRenderObject(RenderObject *masterRenderObject);

	//Renderable ?
	bool IsRenderable() {return renderable;};

	//Dynamic Animation Functions
	void Play(int animDir = 1);
	void Pause();
	void Stop();
	void ToFrame(int frame, int animDir = 1);
	void SetAnimDir(int animDir) { animationDirection = animDir; };
	void Backward() { animationDirection = -1; };
	void Forward() { animationDirection = 1; };
	int GetCurrentFrame() { return curFrame; };

	//Debug
	void DrawRotationCenter();

	void UpdateScale();

	void virtual Update(float x = 0, float y = 0, float rot = 0, float scaleX = 0, float scaleY = 0);

	void virtual Render(bool debug = false, bool frozenFrame = false);

	void virtual FinalRender(float globalCeroX = 0, float globalCeroY = 0, float globalCeroRot = 0, float globalCeroXScale = 1, 
		float globalCeroYScale = 1, bool debug = false, bool frozenFrame = false);
};