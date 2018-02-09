#pragma once

#include <math.h>
#include <stdlib.h>
#include <cstring>

const int WIDTH = 1920;
const int HEIGHT = 1080;
const float mToPx = 50;

enum RESOURCE_ANIM_REFERENCE {NULL_ANIM,
								ASTRONAUT_BODY, FX_THRUST_FIRE, ASTRONAUT_PISTOL,
								COLLECTIBLE_ASTEROID, COLLECTIBLE_COLUMN, 
								BACKGROUND_1, BACKGROUND_MAP, 
								INDICATOR_COMPASS, INDICATOR_FUEL,
								BTN_MAINHATCH,
								CURSOR, CURSOR_ROCKET, 
								ROCKET_ENGINE,
								RESOURCE_ANIM_TOTAL};
enum RESOURCE_FONT_REFERENCE {STANDARD_FONT, RESOURCE_FONT_TOTAL};

enum GAME_STATS {DEBUG_GAME, PLAYER_DISTANCE_TO_CENTER, PLAYER_ANGLE_TO_CENTER, PLAYER_FUEL, GAME_STATS_TOTAL};

enum GAME_STATES {INITIAL, RESOURCE_COLLECTION, INROOM_RESEARCHLAB, INROOM_FORGE, INROOM_STORAGE, MAIN_MENU, GAME_STATES_TOTAL};

enum GAME_OBJECT_TYPES {BUTTON, PLAYER_ASTRONAUT, SPACE_COLLECTIBLE, ROCKET};

enum MOUSE_STATES {DEFAULT, COLLECT_PROPULSOR, COLLECT_PISTOL, COLLECT_PUT_ROCKET, MOUSE_STATES_TOTAL};

enum LAYERS{NO_RENDER, BACKGROUND_LAYER, MAIN_LAYER, FRONT_LAYER, LAYERS_TOTAL};

//Game Initialization
const float PLAYER_INIT_X = 0;
const float PLAYER_INIT_Y = 0;

//General Functions
class GeneralFunctions
{
public:
	double static getAngle(float x1, float y1, float x2, float y2, bool radians = true)
	{
		double angle;
		if (x1 - x2 == 0) angle = 90;
		//CHANGE FOR ATAN2
		else angle = atan2((y1 - y2),(x1 - x2));
		
		if(!radians) angle = angle * (180/3.14);
		return angle;
	};
	double static getDist(float x1, float y1, float x2, float y2)
	{
		double dist = pow(double(pow(x2-x1,2) + pow(y2-y1,2)), 0.5);
		return dist;
	};
	float static randRange(float minNumInit, float maxNumInit, int decimalPlaces = 0)
	{
		int minNum = minNumInit * pow(double (10), decimalPlaces);
		int maxNum = maxNumInit * pow(double (10), decimalPlaces);
		return (float (rand()%(maxNum - minNum + 1) + minNum) / pow(double (10), decimalPlaces));
	};
};