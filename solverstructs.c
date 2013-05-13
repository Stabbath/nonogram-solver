#include "solver.h"

char GetState(Cell* cell) {
	return cell->state;
}

void SetState(Cell* cell, char state) {
	cell->state = state;	
}
/*
void ResetState(Cell* cell) {
	cell->state = STATE_UNKN;
}*/
