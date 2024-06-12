#pragma once
#include "Elements.h"

class Cell {

public:
	Cell();
	~Cell();

	virtual void Update() { return; };

	Element type;
	float temperature;

};