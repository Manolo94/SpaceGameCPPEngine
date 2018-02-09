#include <list>

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <Box2D\Box2D.h>
#include "SimpleBox2D.h"

#include "Game.h"
#include "Globals.h"
#include "ResourcesManager.h"
#include "IndicatorCompass.h"
#include "IndicatorFuel.h"

int main(int argc, char **argv)
{
	//==============================================
	//SHELL VARIABLES
	//==============================================
	bool done = false;
	bool render = false;

	float gameTime = 0;
	int frames = 0;
	int gameFPS = 0;

	//==============================================
	//PROJECT VARIABLES
	//==============================================
	Game thisGame;

	//++++++++++++++++++++++++++++++++++++++++++++++
	//Physics Engine Box2D
	//++++++++++++++++++++++++++++++++++++++++++++++

	// Define the gravity vector.
	b2Vec2 gravity(0.0f, 0.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	b2World world(gravity);

	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and 10 iterations. This provides a high quality simulation
	// in most game scenarios.
	//==============================================
	//ALLEGRO VARIABLES
	//==============================================
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer;

	//DEBUG VARIABLES

	//==============================================
	//ALLEGRO INIT FUNCTIONS
	//==============================================
	if (!al_init())										//initialize Allegro
		return -1;

	display = al_create_display(WIDTH, HEIGHT);			//create our display object

	if (!display) //test display object
	{
		printf("NO DISPLAY!!");
		al_rest(3);
		return -1;
	}
		
	//==============================================
	//ADDON INSTALL
	//==============================================
	al_install_keyboard();
	al_install_mouse();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();


	//==============================================
	//PROJECT INIT
	//==============================================
	thisGame = Game();

	thisGame.Init(&world);
	//------------------Debug Draw------------------------
	AllegroBox2DDebugDraw DebugDraw = AllegroBox2DDebugDraw(thisGame.GetMainCamera());
	world.SetDebugDraw(&DebugDraw);
	DebugDraw.SetFlags(b2Draw::e_shapeBit && b2Draw::e_pairBit && b2Draw::e_jointBit);

	//thisGame.SetGameStat(1, DEBUG_GAME);
	//----------------------------------------------------

	//----------------------------------------------------LOADING----------------------------------------------------------------
	if (!thisGame.GetResourcesManager()->loadAllResources())
	{
		al_show_native_message_box(display, "Onion ENgine", "The engine couldn't load some resources",NULL, NULL, NULL);
		return -1;
	};

	//Creating Indicators
	new IndicatorCompass(0.10*WIDTH, 0.9*HEIGHT, 0, &thisGame);
	new IndicatorFuel(0.95*WIDTH, 0.5*HEIGHT, 0, &thisGame);

	//Creating Background
	new Background(thisGame.GetResourcesManager()->retrieveResource_ANIM(BACKGROUND_1), 0, 0, 0, 1000, 1000, 0.09, thisGame.GetMainCamera());
	//thisGame.GetMainCamera()->AddBackground(new Background(thisGame.GetResourcesManager()->retrieveResource_ANIM_piece(BACKGROUND_MAP, 0, 1000, 0, 500), 600, 400, 0, 1000, 500, 1, false));
	//mainCamera->AddBackground(new Background(bg2Anim, 0, 0, 0, 3000, 3000, 1.0));

	//==============================================
	//TIMER INIT AND STARTUP
	//==============================================
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 60);

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

	al_start_timer(timer);

	gameTime = al_current_time();

	srand(time(NULL));

	//===============================================
	//START GAME
	//===============================================
	thisGame.StartGame();
	
	//Hide Default cursor
	al_hide_mouse_cursor(display);

	while(!thisGame.GetExit())
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		//==============================================
		//INPUT
		//==============================================
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			thisGame.AddKey(ev.keyboard.keycode);
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			//Send to keyboard
			thisGame.removeKey(ev.keyboard.keycode);

			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				// TODO: Make it more generic
				// Toggle between Main Menu and Resource Collection
				if (thisGame.GetGameState() == GAME_STATES::RESOURCE_COLLECTION)
					thisGame.ChangeState(GAME_STATES::MAIN_MENU);
				else
					thisGame.ChangeState(GAME_STATES::RESOURCE_COLLECTION);

				break;
			case ALLEGRO_KEY_D:
				if (thisGame.GetGameStat(DEBUG_GAME) == 0) thisGame.SetGameStat(1, DEBUG_GAME);
				else thisGame.SetGameStat(0, DEBUG_GAME);
				break;
			}
		}
		
		//MOUSE BUTTON
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (ev.mouse.button == 1) thisGame.GetMouse()->LeftClickDown();
			if (ev.mouse.button == 2) thisGame.GetMouse()->RightClickDown();
		}
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
		{
			if (ev.mouse.button == 1) thisGame.GetMouse()->LeftClickUp();
			if (ev.mouse.button == 2) thisGame.GetMouse()->RightClickUp();
		}

		//MOUSE MOVE
		else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES)
		{
			thisGame.UpdateMouse(ev.mouse.x, ev.mouse.y);
		}

		//==============================================
		//GAME UPDATE
		//==============================================
		else if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			render = true;

			//printf("%d\n", thisGame.keys.size());

			//UPDATE FPS===========
			frames++;
			if(al_current_time() - gameTime >= 1)
			{
				gameTime = al_current_time();
				gameFPS = frames;
				frames = 0;
			}
			//=====================
			thisGame.Update(thisGame.GetMouse()->GetX(), thisGame.GetMouse()->GetY());
		}

		//==============================================
		//RENDER
		//==============================================
		if(render && al_is_event_queue_empty(event_queue))
		{
			render = false;

			//BEGIN PROJECT RENDER================
			thisGame.Render();

			//DebugDraw
			if(thisGame.GetGameStat(DEBUG_GAME) == 1) world.DrawDebugData();

			//FLIP BUFFERS========================
			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
	}

	//==============================================
	//DESTROY PROJECT OBJECTS
	//==============================================*/
	//Bitmaps
	thisGame.GetResourcesManager()->ClearAll();

	//SHELL OBJECTS=================================
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);

	return 0;
}