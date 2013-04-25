#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stacks.h"
#include "solver.h"

//note: check to make sure that sum of blocks + blanks is <= line length in getPuzzle: if a line is not, just quit since puzzle is impossible

Line* GetLeftmost(Line* old, int length) {
	Line* line = CloneLine(old, length);
	
	int i, j;
	for (i = 0, j = 0; i < line->blockNum; i++) {
		while (j < line->block[i].length)
			line->cells[j++]->state = STATE_FULL;
		if (j < length)
			line->cells[j++]->state = STATE_BLNK;
	}
	while (j < length)
		line->cells[j++]->state = STATE_UNKN;
	
	return line;
}

Line* GetRightmost(Line* old, int length) {
	Line* line = CloneLine(old, length);

	int i, j = 0;
	int offset = length - getMinSumOfBlocksAndBlanks(line);
	while (j < offset)
		line->cells[j++]->state = STATE_UNKN;

	for (i = 0; i < line->blockNum; i++) {
		while (j - offset < line->block[i].length)
			line->cells[j++]->state = STATE_FULL;
		if (j < length)
			line->cells[j++]->state = STATE_BLNK;
	}
	
	return line;
}

Line* CloneLine(Line* old, int length) {
	Line* new = (Line*) malloc(sizeof(Line));
	int i;
	new->blockNum = old->blockNum;
	new->block = (Block*) malloc(new->blockNum*sizeof(Block));
	for (i = 0; i < new->blockNum; i++) {
		new->block[i] = old->block[i];
	}
	new->cells = (Cell**) malloc(length*sizeof(Cell*));
	for (i = 0; i < length; i++) {
		new->cells[i] = (Cell*) malloc(sizeof(Cell));
		new->cells[i]->state = old->cells[i]->state;
	}
	return new;
}

void FreeLine(Line* line) {

}

void FreePuzzle(Puzzle* puzzle) {


	
/*	free(puzzle->length);
	free(puzzle->line[ROW]);
	free(puzzle->line[COL]);
	free(puzzle->line);
	free(puzzle);*/
}

Puzzle* ClonePuzzle(Puzzle* old) {
	Puzzle* puzzle = (Puzzle*) malloc(sizeof(Puzzle));

	puzzle->length = (int*) malloc(AXES*sizeof(int));
	puzzle->length[ROW] = old->length[ROW];
	puzzle->length[COL] = old->length[COL];
	
	puzzle->line = (Line**) malloc(AXES*sizeof(Line*));
	puzzle->line[ROW] = (Line*) malloc(puzzle->length[ROW]*sizeof(Line));
	puzzle->line[COL] = (Line*) malloc(puzzle->length[COL]*sizeof(Line));

	int i, j;
	for (i = 0; i < puzzle->length[ROW]; i++) {
		puzzle->line[ROW][i].blockNum = old->line[ROW][i].blockNum;
		puzzle->line[ROW][i].block = (Block*) malloc(puzzle->line[ROW][i].blockNum*sizeof(Block));
		for (j = 0; j < puzzle->line[ROW][i].blockNum; j++) {
			puzzle->line[ROW][i].block[j].length = old->line[ROW][i].block[j].length;
		}

		puzzle->line[ROW][i].cells = (Cell**) malloc(puzzle->length[COL]*sizeof(Cell*));
		for (j = 0; j < puzzle->length[COL]; j++) {
			puzzle->line[ROW][i].cells[j] = (Cell*) malloc(sizeof(Cell));
			puzzle->line[ROW][i].cells[j]->state = old->line[ROW][i].cells[j]->state;
		}
	}
	for (i = 0; i < puzzle->length[COL]; i++) {
		puzzle->line[COL][i].blockNum = old->line[COL][i].blockNum;
		puzzle->line[COL][i].block = (Block*) malloc(puzzle->line[COL][i].blockNum*sizeof(Block));
		for (j = 0; j < puzzle->line[COL][i].blockNum; j++) {
			puzzle->line[COL][i].block[j].length = old->line[COL][i].block[j].length;
		}

		puzzle->line[COL][i].cells = (Cell**) malloc(puzzle->length[ROW]*sizeof(Cell*));
		for (j = 0; j < puzzle->length[ROW]; j++) {
			puzzle->line[COL][i].cells[j] = puzzle->line[ROW][j].cells[i];
		}
	}
	
	return puzzle;
}

int getMinSumOfBlocksAndBlanks(Line* line) {
	int i, count;
	for (i = 0, count = 0; i < line->blockNum; i++) {
		count += line->block[i].length;
	}
	if (line->blockNum > 1) {
		count += 1*(line->blockNum - 1);
	}
	return count;
}

int getLengthOfLargestBlock(Line* line) {
	int i, size;
	for (i = 0, size = 0; i < line->blockNum; i++) {
		if (line->block[i].length > size)	size = line->block[i].length;
	}
	return size;
}

//is there a point in having these special cases?
int stackline(Puzzle* puzzle, Line* line, Stack* stack, int coord) {
	int sum = getMinSumOfBlocksAndBlanks(line);
	int cap = getLengthOfLargestBlock(line);
			
	if (sum > puzzle->length[coord]) {
		return - puzzle->length[ROW] * puzzle->length[COL];
	}
	
	int i, n;
	if (sum == 0) {	//means the whole line has blanks (special case)
		for (i = 0, n = 0; i < puzzle->length[coord]; i++) {
			if (line->cells[i]->state == STATE_UNKN) {
				line->cells[i]->state = STATE_BLNK;
				n++;
			}
		}
		return n;
	} else
	if (/*puzzle->length[coord] == sum*/1) {	//means the right-most arrangement is the same as the left-most: full solution is known (special case)
//		printf("\t\tnot 0\n");//go through block lengths
	} //else - commented since there's no code for the special case yet*/
	if (puzzle->length[coord] - sum < cap) {	//only situation in which stacking will do anything
		Line* left = GetLeftmost(line, puzzle->length[coord]);
		Line* right = GetRightmost(line, puzzle->length[coord]);
		
		int opCoord = (coord == ROW ? COL : ROW);
		for (i = 0, n = 0; i < puzzle->length[coord]; i++) {
			if (left->cells[i]->state == right->cells[i]->state) {
				if (line->cells[i]->state == STATE_UNKN) {
					line->cells[i]->state = left->cells[i]->state;
					n++;
					if (!IsInStack(stack, &puzzle->line[opCoord][i]))
						Push(stack, (void*) &puzzle->line[opCoord][i]);
				}
			}
		}
		FreeLine(left);
		FreeLine(right);
		return n;
	}	
	return 0;
}

int solveline(Puzzle* puzzle, Line* line, Stack* stack, int coord) {
	int n = stackline(puzzle, line, stack, coord);
	return n;
}

Cell* PickCell(Puzzle* puzzle) {
	int i, j;
	for (i = 0; i < puzzle->length[ROW]; i++) {
		for (j = 0; j < puzzle->length[COL]; j++) {
			if (puzzle->line[ROW][i].cells[j]->state == STATE_UNKN) {
				return puzzle->line[ROW][i].cells[j];
			}
		}
	}
	return NULL;
}

void solve(Puzzle* puzzle, Stack** stack, int unsolvedCellCount) {
	while ((!IsStackEmpty(stack[ROW]) || !IsStackEmpty(stack[COL])) && unsolvedCellCount > 0) {
		if (!IsStackEmpty(stack[ROW])) {
			unsolvedCellCount -= solveline(puzzle, Pop(stack[ROW]), stack[COL], ROW);		
		} else {
			unsolvedCellCount -= solveline(puzzle, Pop(stack[COL]), stack[ROW], COL);
		}
	}
	
	ExportSolution(puzzle);
	
	if (unsolvedCellCount > 0) {
/*		Puzzle* newPuzzle;

		int row = 0, col = 0;
		Cell* pick;	//note: the way this is currently implemented, there can be no "random" elements in the picking of cells: the same puzzle must always lead to the same cell getting picked
		
		newPuzzle = ClonePuzzle(puzzle);
		if ((pick = PickCell(newPuzzle)) == NULL) return;
		pick->state = STATE_FULL;
		Push(stack[ROW], (void*) &newPuzzle->line[ROW][row]);
		Push(stack[COL], (void*) &newPuzzle->line[COL][col]);
		solve(newPuzzle, stack, unsolvedCellCount - 1);
		FreePuzzle(newPuzzle);
		
		newPuzzle = ClonePuzzle(puzzle);
		if ((pick = PickCell(newPuzzle)) == NULL) return;
		pick->state = STATE_BLNK;
		Push(stack[ROW], (void*) &newPuzzle->line[ROW][row]);
		Push(stack[COL], (void*) &newPuzzle->line[COL][col]);
		solve(newPuzzle, stack, unsolvedCellCount - 1);
		FreePuzzle(newPuzzle);*/
	} else
	if (unsolvedCellCount == 0) {
		ClearStack(stack[ROW]);	//could just not push perpendicular lines that are already
		ClearStack(stack[COL]);	//solved instead of clearing stacks here

//		ExportSolution(puzzle);
	} else {
		//invalid solution, get out
	}
	
	return;
}

//possibly use a single stack, defining the axis that's currently being used in a static int in a short function, instead of repeatedly passing it along
//is it really necessary to have 2 stacks? they store line pointers, so it shouldn't matter if it's a row or a column
int main (int n, char** args) {
	if (n < 2) errorout(ERROR_ARGS, "No file name was given.");	
	
	Puzzle* puzzle = getPuzzle(args[1]);
	
	int i;
	
	Stack** stack = (Stack**) malloc(AXES*sizeof(Stack*));
	for (i = 0; i < AXES; i++) stack[i] = CreateStack();
	for (i = 0; i < puzzle->length[ROW]; i++)	Push(stack[ROW], (void*) &puzzle->line[ROW][i]);
	for (i = 0; i < puzzle->length[COL]; i++)	Push(stack[COL], (void*) &puzzle->line[COL][i]);

	solve(puzzle, stack, puzzle->length[ROW] * puzzle->length[COL]);
	FreePuzzle(puzzle);
	ClearStack(stack[ROW]);
	ClearStack(stack[COL]);
	free(stack[ROW]);
	free(stack[COL]);
	free(stack);
	ExportSolution(NULL);
	
	return 0;	
}
