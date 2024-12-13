#pragma once

#include <cstdlib>
#include <unordered_map>

enum Element{EMPTY, BORDER, SAND, WATER, WOOD, STONE, LAVA, LAVA_STONE, FIRE, EMBER, ICE, ASH, MAGIC_DUST,
	SMOKE, SNOW, STEAM, ACID, CERAMIC, CHAR, SEED, LEAF, DIRT, LUCIFERIN, LIGHTNING_START, 
	LIGHTNING, GOLD, MOLTEN_GOLD, JADE, NR_ELEMENTS
};

enum Element_Type { SOLID, LIQUID, GAS };

static std::unordered_map < Element, Element_Type> Type_Map = {

	{FIRE, GAS},
	{STEAM, GAS},
	{SMOKE, GAS},
	{WATER, LIQUID},
	{LAVA, LIQUID},
	{MOLTEN_GOLD, LIQUID},
	{LUCIFERIN, LIQUID}
};

typedef struct color_t {

	float r, g, b, a;

}color_t;

typedef struct vector_t {
	int x, y;
}vector_t;

static color_t empty_col{ 0, 0, 0 };
static color_t border_col{ 0, 0, 0 };
static color_t sand_col{ 194.f, 177.f, 95.f };
static color_t water_col{ 48.f, 135.f, 255.f};
static color_t wood_col{ 71, 41, 14 };
static color_t stone_col{ 130, 125, 120 };
static color_t lava_col{ 212, 80, 19 };
static color_t fire_col{ 212, 120, 70 };
static color_t smoke_col{ 44, 44, 44 };
static color_t burn_col{ 255, 161, 38 };


static float randomFloat()
{
	return (float)(rand()) / (float)(RAND_MAX);
}

static const float randomBetween(float from, float to)
{
	float diff = to - from;
	return (((float)rand() / RAND_MAX) * diff) + from;
}