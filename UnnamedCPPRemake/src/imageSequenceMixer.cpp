#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

void convertImage()
{	
	int frameWidth = 35;
	int frameHeight = 50;

	int columns = 5;
	int rows = 2;

	ALLEGRO_BITMAP *newImage = al_create_bitmap(frameWidth*columns,frameHeight*rows);

	al_init_image_addon();
	
	ALLEGRO_BITMAP *images[11];
	images[0] = al_load_bitmap("fire0000.png");
	images[1] = al_load_bitmap("fire0001.png");
	images[2] = al_load_bitmap("fire0002.png");
	images[3] = al_load_bitmap("fire0003.png");
	images[4] = al_load_bitmap("fire0004.png");
	images[5] = al_load_bitmap("fire0005.png");
	images[6] = al_load_bitmap("fire0006.png");
	images[7] = al_load_bitmap("fire0007.png");
	images[8] = al_load_bitmap("fire0008.png");
	images[9] = al_load_bitmap("fire0009.png");
	images[10] = al_load_bitmap("fire0010.png");

	al_set_target_bitmap(newImage);

	al_clear_to_color(al_map_rgb(78,95,38));

	for(int i = 0; i <= 10; i++)
	{
		al_draw_bitmap(images[i], (i%columns)*frameWidth, (i/columns)*frameHeight, 0);
	}

	al_convert_mask_to_alpha(newImage, al_map_rgb(78,95,38));

	al_save_bitmap("fireAnim.png", newImage);

	for(int i = 0; i <= 10; i++)
	{
		al_destroy_bitmap(images[i]);
	}

	al_destroy_bitmap(newImage);
};