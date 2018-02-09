#pragma once

#include "RenderObject.h"

class Camera;

class Background : public RenderObject
{
private:
	float relativeSpeed;

	Camera *masterCamera;
	
	bool tiled;

public:
	Background(Animation *animation, int x, int y, float rotation, float width, float height, float relativeSpeed, Camera *masterCamera, bool tiled = true);

	void drawBackgroundPortion(float startCutPointX, float endCutPointX, float startCutPointY, float endCutPointY, float drawPointX, float drawPointY); 

	void virtual Render(bool debug = false, bool frozenFrame = false);
};