#pragma once

#include <Box2D\Box2D.h>
#include <allegro5\allegro5.h>
#include <allegro5/allegro_primitives.h>



class Camera;

class AllegroBox2DDebugDraw : public b2Draw
{
private:
	
public:
	AllegroBox2DDebugDraw(Camera* camera);

	Camera* alDebugCamera;

	/// Draw a closed polygon provided in CCW order.
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a solid closed polygon provided in CCW order.
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a circle.
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	
	/// Draw a solid circle.
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	
	/// Draw a line segment.
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	void DrawTransform(const b2Transform& xf);

	/// Draw a point.
	virtual void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
};