#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <list>
#include "Globals.h"
#include "SimpleBox2D.h"

class Game;
class RenderObject;
class Animation;
class ResourcesManager;

class GameObject
{
private:
	int ID;
	GAME_OBJECT_TYPES TYPE;
	bool touched;
	bool renderable;
protected:
	RenderObject *renderObject;
	b2Body *physicsObject;

	Game *masterGame;
public:
	//Default constructor
	GameObject();
	//Destroyer
	void virtual Destroy();
	//Initializer
	void Init(Animation *animation, float x, float y, float width, float height, b2World *world, Game *game, int renderLayer, bool staticPhysics = false);

	//Update
	void virtual Update(float cameraXOffset = 0, float cameraYOffset = 0, bool mouseIntegration = false, float mouseX = 0, float mouseY = 0);
	//Render
	void virtual Render(float globalCeroX = 0, float globalCeroY = 0, float globalCeroRot = 0, float globalCeroXScale = 1, float globalCeroYScale = 1);

	//Mouse entering Function
	void virtual OnEnter(){};

	//Mouse exiting Function
	void virtual OnExit(){};

	//Mouse clicking Function
	void virtual OnClick(){};

	//Render Object
	RenderObject *GetRenderObject() {return GameObject::renderObject;};
	//Physics Object
	b2Body *GetPhysicsObject() {return GameObject::physicsObject;}

	//TYPE
	GAME_OBJECT_TYPES GetType() {return TYPE;}
	void SetType(GAME_OBJECT_TYPES newTYPE) {GameObject::TYPE = newTYPE;}

	//Renderable
	bool GetRenderable() {return renderable;};
	void SetRenderable(bool renderable) {GameObject::renderable = renderable;};
};