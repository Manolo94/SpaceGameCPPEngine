#include "AllegroBox2DDebugDraw.h"
#include "Camera.h"

AllegroBox2DDebugDraw::AllegroBox2DDebugDraw(Camera *camera)
{
	alDebugCamera = camera;
};

void AllegroBox2DDebugDraw::DrawPolygon(const b2Vec2* vertices, const int32 vertexCount, const b2Color& color) 
{
	ALLEGRO_VERTEX v[16];

	for(int o = 0; o < vertexCount; o++)
	{
		v[o].color = al_map_rgb(color.r*255, color.g*255, color.b*255);
		v[o].x = (vertices[o].x - alDebugCamera->focusX)*mToPx + WIDTH/2;
		v[o].y = (vertices[o].y - alDebugCamera->focusY)*mToPx + HEIGHT/2;
		v[o].z = 0;
		v[o].u = v[o].x;
		v[o].v = v[o].y;
	}

	al_draw_prim(v, NULL, NULL, 0, vertexCount, ALLEGRO_PRIM_LINE_LOOP);}

void AllegroBox2DDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) 
{
	ALLEGRO_VERTEX v[16];

	for(int o = 0; o < vertexCount; o++)
	{
		v[o].color = al_map_rgb(color.r*255, color.g*255, color.b*255);
		v[o].x = (vertices[o].x - alDebugCamera->focusX)*mToPx + WIDTH/2;
		v[o].y = (vertices[o].y - alDebugCamera->focusY)*mToPx + HEIGHT/2;
		v[o].z = 0;
		v[o].u = v[o].x;
		v[o].v = v[o].y;
	}

	al_draw_prim(v, NULL, NULL, 0, vertexCount, ALLEGRO_PRIM_TRIANGLE_FAN);
}

void AllegroBox2DDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) 
{
	al_draw_circle((center.x - alDebugCamera->focusX)*mToPx + WIDTH/2, 
							(center.y - alDebugCamera->focusY)*mToPx + HEIGHT/2, radius*mToPx, 
							al_map_rgb(color.r*255, color.g*255, color.b*255), 1);
};
void AllegroBox2DDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) 
{
	al_draw_filled_circle((center.x - alDebugCamera->focusX)*mToPx + WIDTH/2, 
							(center.y - alDebugCamera->focusY)*mToPx + HEIGHT/2, radius*mToPx, 
							al_map_rgb(color.r*255, color.g*255, color.b*255));
};

void AllegroBox2DDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) 
{
	al_draw_line((p1.x - alDebugCamera->focusX)*mToPx + WIDTH/2, 
		(p1.y - alDebugCamera->focusY)*mToPx + HEIGHT/2, 
		(p2.x - alDebugCamera->focusX)*mToPx + WIDTH/2, 
		(p2.y - alDebugCamera->focusY)*mToPx + HEIGHT/2, 
		al_map_rgb(color.r*255, color.g*255, color.b*255), 1);
}

void AllegroBox2DDebugDraw::DrawTransform(const b2Transform& xf) 
{
	b2Vec2 p1 = xf.p, p2;
	const float32 k_axisScale = 0.4f;

	p2 = p1 + k_axisScale * xf.q.GetXAxis();
    AllegroBox2DDebugDraw::DrawSegment(p1,p2,b2Color(1,0,0));

	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	AllegroBox2DDebugDraw::DrawSegment(p1,p2,b2Color(0,1,0));
};

void AllegroBox2DDebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	al_draw_filled_circle((p.x - alDebugCamera->focusX)*mToPx + WIDTH / 2,
		(p.y - alDebugCamera->focusY)*mToPx + HEIGHT / 2, 2*mToPx,
		al_map_rgb(color.r * 255, color.g * 255, color.b * 255));
}