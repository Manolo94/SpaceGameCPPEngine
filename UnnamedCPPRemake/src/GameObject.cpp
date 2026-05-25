#include "GameObject.h"

#include "Game.h"
#include "RenderObject.h"
#include "Animation.h"

GameObject::GameObject()
{
	renderable = true;
}

void GameObject::Destroy()
{

};

void GameObject::Init(Animation *animation, float x, float y, float width, float height, b2World *world, Game *game, int renderLayer, bool staticPhysics)
{
	GameObject::renderObject = new RenderObject();
	
	GameObject::renderObject->Init(animation, x, y, 0, width, height, game, renderLayer);
	
	GameObject::masterGame = game;
	//Initialize physicsObject
	physicsObject = SimpleBox2D::preparePhysicsObject(x, y, world, staticPhysics);
	
	GameObject::masterGame->GetMainCamera()->GameObjectList.push_front(this);
};

void GameObject::Update(float cameraXOffset, float cameraYOffset, bool mouseIntegration, float mouseX, float mouseY)
{
	GameObject::renderObject->SetX((GameObject::physicsObject->GetPosition().x - cameraXOffset)*mToPx);
	GameObject::renderObject->SetY((GameObject::physicsObject->GetPosition().y - cameraYOffset)*mToPx);
	GameObject::renderObject->SetRotation(GameObject::physicsObject->GetAngle());

	//Out of bounds
	if (GameObject::renderObject->GetX() + GameObject::renderObject->GetWidth() + GameObject::renderObject->GetHeight() < 0 ||
		GameObject::renderObject->GetX() - GameObject::renderObject->GetWidth() - GameObject::renderObject->GetHeight() > WIDTH)
		renderable = false;
	else if (GameObject::renderObject->GetY() + GameObject::renderObject->GetWidth() + GameObject::renderObject->GetHeight() < 0 ||
		GameObject::renderObject->GetY() - GameObject::renderObject->GetWidth() - GameObject::renderObject->GetHeight() > HEIGHT)
		renderable = false;
	else renderable = true;

	//Whenever the mouse is on this GameObject
	if (GameObject::physicsObject->GetFixtureList()->TestPoint(b2Vec2(mouseX / mToPx + cameraXOffset, mouseY / mToPx + cameraYOffset)))
	{
		//Call OnEnter Function
		OnEnter();

		masterGame->GetMouse()->SetTouchingObject(this);
	}
	//Whenever the mouse leaves this GameObject
	else OnExit();
};

void GameObject::Render(float globalCeroX, float globalCeroY, float globalCeroRot, float globalCeroXScale, float globalCeroYScale)
{
	//if (renderable)
	//	renderObject->Render(globalCeroX, globalCeroY, globalCeroRot, globalCeroXScale, globalCeroYScale);
};