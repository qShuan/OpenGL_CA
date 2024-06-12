#include "Sand.h"

Sand::Sand(){
	type = SAND;
	temperature = 36.6f;
}

Sand::~Sand() {}

void Sand::Update() {
	type = BORDER;
}