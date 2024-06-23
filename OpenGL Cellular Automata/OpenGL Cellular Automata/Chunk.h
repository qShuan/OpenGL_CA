#pragma once
#include "Elements.h"

class Chunk {

public:

	Chunk();
	Chunk(vector_t topLeft, vector_t bottomRight);
	~Chunk();


	bool shouldUpdate = true;
	bool shouldUpdateNextFrame = true;

	vector_t topLeft;
	vector_t topRight;
	vector_t bottomRight;
	vector_t bottomLeft;

	void setTopLeft(vector_t topLeft);
	void setBottomLeft(vector_t bottomLeft);
	void setBottomRight(vector_t bottomRight);
	void setTopRight(vector_t topRight);
	void shiftUpdatesAndReset();
};