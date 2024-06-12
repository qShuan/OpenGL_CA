#pragma once
#include "Cell.h"

class Sand: public Cell {

public:
	Sand();
	~Sand();

	void Update() override;
};