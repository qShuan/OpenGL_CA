#include "Cells.h"
#include <random>
#include "HSL.h"
#include <iostream>

float RandomFloat(float min, float max)
{
	return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}

color_t RandomizeColor(color_t color) {

	HSL hsl = TurnToHSL(color);

	float r_L = RandomFloat(-10.f, 0.f);
	float r_S = RandomFloat(-10.f, 0.f);

	hsl.Hue = floor(hsl.Hue);
	hsl.Luminance += r_L;
	hsl.Saturation += r_S;


	color = hsl.TurnToRGB();

	return color;
}

cell_t cell_empty() {

	cell_t p = { EMPTY };

	p.color = empty_col;
	p.temperature = 20.f;

	return p;
}
cell_t cell_border() {

	cell_t p = { BORDER };

	p.color = border_col;
	p.temperature = 0.f;

	return p;
}
cell_t cell_sand() {

	cell_t p = { SAND };

	p.color = RandomizeColor(sand_col);
	p.temperature = randomBetween(30.f, 36.f);

	p.life = randomBetween(2.f, 5.f);

	return p;
}
cell_t cell_water() {

	cell_t p = { WATER };

	p.color = RandomizeColor(water_col);
	p.temperature = randomBetween(10.f, 25.f);

	p.life = randomBetween(10.f, 25.f);

	return p;
}
cell_t cell_wood() {

	cell_t p = { WOOD };

	p.color = RandomizeColor(wood_col);
	p.temperature = randomBetween(20.f, 22.f);

	p.life = randomBetween(20.f, 30.f);

	return p;
}
cell_t cell_stone() {

	cell_t p = { STONE };

	p.color = RandomizeColor(stone_col);
	p.temperature = randomBetween(30.f, 36.f);

	p.life = randomBetween(35.f, 50.f);

	return p;
}
cell_t cell_lava() {

	cell_t p = { LAVA };

	p.color = RandomizeColor(lava_col);
	p.temperature = randomBetween(1000.f, 1200.f);

	p.life = randomBetween(15.f, 30.f);

	return p;
}

cell_t cell_fire() {

	cell_t p = { FIRE };

	p.color = RandomizeColor(fire_col);
	p.temperature = randomBetween(900.f, 1000.f);

	p.life = randomBetween(15.f, 30.f);

	return p;
}
cell_t cell_smoke() {

	cell_t p = { SMOKE };

	p.color = RandomizeColor(smoke_col);
	p.temperature = randomBetween(26.f, 38.f);

	p.life = randomBetween(15.f, 30.f);

	return p;
}

cell_t cell_current(Element& type) {

	switch (type) {

	case EMPTY:
		return cell_empty();
	case BORDER:
		return cell_border();
	case SAND:
		return cell_sand();
	case WATER:
		return cell_water();
	case WOOD:
		return cell_wood();
	case STONE:
		return cell_stone();
	case LAVA:
		return cell_lava();
	case FIRE:
		return cell_fire();
	case SMOKE:
		return cell_smoke();
	}

	return cell_empty();
}