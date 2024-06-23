#include "Chunk.h"

Chunk::Chunk() {}

Chunk::~Chunk() {}

Chunk::Chunk(vector_t topLeft, vector_t bottomRight) {

	this->topLeft = topLeft;
	this->bottomRight = bottomRight;
}

void Chunk::setTopLeft(vector_t topLeft) {

	this->topLeft = topLeft;
}

void Chunk::setTopRight(vector_t topRight) {

	this->topRight = topRight;
}

void Chunk::setBottomRight(vector_t bottomRight) {

	this->bottomRight = bottomRight;
}
void Chunk::setBottomLeft(vector_t bottomLeft) {

	this->bottomLeft = bottomLeft;
}
void Chunk::shiftUpdatesAndReset() {

	this->shouldUpdate = this->shouldUpdateNextFrame;
	this->shouldUpdateNextFrame = false;
}