#pragma once

#include "RenderObject.h"
#include "Globals.h"
#include <allegro5\allegro_font.h>

class Indicator : public RenderObject
{
protected:
	ALLEGRO_FONT *indicatorFont;
public:	
	void virtual Render(bool debug = false, bool frozenFrame = false){};
};