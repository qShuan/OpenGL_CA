#pragma once
#include "Elements.h"

typedef struct velocity_t {

	float x, y;
}velocity_t;

typedef struct cell_t {

	Element type;

	color_t color;
	velocity_t velocity;

	//State properties
	float temperature;
	bool isBurning;
	bool isFalling = true;
	float life;
	bool moved_last_frame;

}cell_t;

float RandomFloat(float min, float max);
color_t RandomizeColor(color_t color);


cell_t cell_empty();
cell_t cell_border();
cell_t cell_sand();
cell_t cell_water();
cell_t cell_wood();
cell_t cell_stone();
cell_t cell_lava();
cell_t cell_lavaStone();
cell_t cell_ember();
cell_t cell_fire();
cell_t cell_ice();
cell_t cell_ash();
cell_t cell_magicDust();
cell_t cell_smoke();
cell_t cell_snow();
cell_t cell_steam();
cell_t cell_acid();
cell_t cell_ceramic();
cell_t cell_char();
cell_t cell_seed();
cell_t cell_leaf();
cell_t cell_dirt();
cell_t cell_luciferin();
cell_t cell_lightning_start();
cell_t cell_lightning();
cell_t cell_gold();
cell_t cell_jade();

cell_t cell_current(Element& type);