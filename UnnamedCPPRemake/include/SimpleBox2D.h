#pragma once

#include "Globals.h"
#include <Box2D/Box2D.h>
#include "AllegroBox2DDebugDraw.h"

class SimpleBox2D
{
public:
	b2Body static *preparePhysicsObject(float x, float y, b2World *world, bool staticObject = false)
	{
		// Create the physics Object. We set its position and call the body factory.
		b2BodyDef* gameObjectBodyDef = new b2BodyDef();
		gameObjectBodyDef->type = b2_dynamicBody;
		if (staticObject) gameObjectBodyDef->type = b2_staticBody;
		gameObjectBodyDef->position.Set(x/mToPx, y/mToPx);
		return world->CreateBody(gameObjectBodyDef);
	};

	void static dynamicCircle(b2Body *dynamicCircleBody, float radius, b2World *world, bool sensor = false)
	{
		// Define another box shape for our dynamic body.
		b2CircleShape dynamicCircle;
		dynamicCircle.m_radius = radius/mToPx;
		
		// Define the dynamic body fixture.
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicCircle;

		if(sensor)
		{
			fixtureDef.isSensor = true;
		}
		else
		{
			// Set the box density to be non-zero, so it will be dynamic.
			fixtureDef.density = 1.0f;

			// Override the default friction.
			fixtureDef.friction = 0.3f;
		};

		// Add the shape to the body.
		dynamicCircleBody->CreateFixture(&fixtureDef);
	};		

	void static dynamicRectangle(b2Body *dynamicRectangleBody, float width, float height, b2World *world)
	{
		//PHYSICS OBJECT SETUP
		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(width/2/mToPx, height/2/mToPx);

		// Define the dynamic body fixture.
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;

		// Set the box density to be non-zero, so it will be dynamic.
		fixtureDef.density = 1.0f;

		// Override the default friction.
		fixtureDef.friction = 0.3f;

		// Add the shape to the body.
		dynamicRectangleBody->CreateFixture(&fixtureDef);
	};

	void static SetLinearVelocity(b2Body *dynamicBody, float xVel, float yVel)
	{
		dynamicBody->SetLinearVelocity(b2Vec2(xVel, yVel));
	};
};

class RaycastHandler : public b2RayCastCallback
{
	//Whether the raycasthandler had already reported a fixture or not
	bool hit;
	//Whether the raycasthandler is looking for an specific fixture or not
	bool seeking;
	//Target Fixture
	b2Fixture* targetFixture;

	//Fixture reported
	b2Fixture* fixtureHit;
	//Point Reported
	b2Vec2 pointHit;
	//Normal reported
	b2Vec2 normalHit;

public:
	//Costructor
	RaycastHandler()
	{
		hit = false;
		seeking = false;
	};

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		//Declare its been hit
		hit = true;

		if (seeking)
		{
			if (fixture == targetFixture)
			{
				//Retrieve all the info
				fixtureHit = fixture;
				pointHit = point;
				normalHit = normal;

				//printf("frac %f", fraction);
				return fraction;
			}
			else return 1;
		}
		else return fraction;
	};

	//Set target object
	void SetTargetFixture(b2Fixture* target) 
	{
		seeking = true;
		targetFixture = target; 
	};

	//Get the normal angle from the last hit detected
	float GetNormalAngle()
	{
		float angle = atan(normalHit.y / normalHit.x);

		if (normalHit.x < 0 && normalHit.y < 0) angle = angle + 3.14;
		if (normalHit.x < 0 && normalHit.y > 0) angle = angle + 3.14;

		if (hit) return angle;
		else return 0;
	};

	//Get the X of the last hit detected
	float GetPointX()
	{
		if (hit) return pointHit.x;
		else return 0;
	};

	//Get the Y of the last hit detected
	float GetPointY()
	{
		if (hit) return pointHit.y;
		else return 0;
	};
};