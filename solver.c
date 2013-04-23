#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stacks.h"
#include "solver.h"

void LineCopy(Line* line1, Line* line2, int length) {
	int i;
	for (i = 0; i < length; i++) {
		line2[i] = line1[i];
	}
}

Puzzle* CopyPuzzle(Puzzle* old) {
	Puzzle* puzzle = (Puzzle*) malloc(sizeof(Puzzle));
	
	puzzle->length = (int*) malloc(AXES*sizeof(int));
	puzzle->length[ROW] = old->length[ROW];
	puzzle->length[COL] = old->length[COL];
	
	puzzle->line = (Line**) malloc(AXES*sizeof(Line*));
	puzzle->line[ROW] = (Line*) malloc(puzzle->length[ROW]*sizeof(Line));
	puzzle->line[COL] = (Line*) malloc(puzzle->length[COL]*sizeof(Line));
	int i, j, k;
	for (k = ROW; k != COL; k = COL) {
		for (i = 0; i < puzzle->length[ROW]; i++) {
			puzzle->line[k][i].blockNum = old->line[k][i].blockNum;
			puzzle->line[k][i].block = (Block*) malloc(puzzle->line[k][i].blockNum*sizeof(Block));
			for (j = 0; j < puzzle->line[k][i].blockNum; j++) {
				puzzle->line[k][i].block[j].length = old->line[k][i].block[j].length;
			}

			puzzle->line[k][i].cells = (Cell**) malloc(puzzle->length[k]*sizeof(Cell*));
			for (j = 0; j < puzzle->length[k]; j++) {
				puzzle->line[k][i].cells[j] = (Cell*) malloc(sizeof(Cell));
				old->line[k][i].cells[j]
			}
		}
	}
	
	return puzzle;
}

int getMinSumOfBlocksAndBlanks(Puzzle* puzzle, Line* line, int coord) {
	int i, count;
	for (i = 0, count = 0; i < puzzle->length[coord]; i++) {
		count += line->block[i].length;
	}
	return count;
}

int getLengthOfLargestBlock(Puzzle* puzzle, Line* line, int coord) {
	int i, size;
	for (i = 0, size = 0; i < puzzle->length[coord]; i++) {
		if (line->block[i].length > size)	size = line->block[i].length;
	}
	return size;
}
 
 //is there a point in having these special cases?
int stackline(Puzzle* puzzle, Line* line, Stack* stack, int coord) {
	int sum = getMinSumOfBlocksAndBlanks(puzzle, line, coord);
	int cap = getLengthOfLargestBlock(puzzle, line, coord);
	if (sum == 0) {	//means the whole line has blanks (special case)
	printf("t\n");
	fflush(stdout);
		int i;
		for (i = 0; i < puzzle->length[coord]; i++) {
			line->cells[i]->state = STATE_BLNK;
			return puzzle->length[coord];
		}
	} else
	if (puzzle->length[coord] == sum) {	//means the right-most arrangement is the same as the left-most: full solution is known (special case)
		//go through block lengths
	} //else commented since there's no code for the special case yet
	if (puzzle->length[coord] - sum < cap) {	//only situation in which stacking will do anything
		Line* left = (Line*) malloc(sizeof(Line));
		Line* right = (Line*) malloc(sizeof(Line));
		LineCopy(left, line, puzzle->length[coord]);
		LineCopy(right, line, puzzle->length[coord]);
		
		int i, n;
		for (i = 0; i < puzzle->length[coord]; i++) {
			printf("%c vs %c", left->cells[i]->state, right->cells[i]->state);
			if (left->cells[i]->state == right->cells[i]->state) {
				line->cells[i]->state = left->cells[i]->state;
				n++;
			}
		}
		return n;
	}
	return 0;
}

int solveline(Puzzle* puzzle, Line* line, Stack* stack, int coord) {
	int n = stackline(puzzle, line, stack, coord);
	return n;
}

void solve(Puzzle* puzzle, Stack** stack, int unsolvedCellCount) {
	while ((!IsStackEmpty(stack[ROW]) || !IsStackEmpty(stack[COL])) && unsolvedCellCount > 0) {
		unsolvedCellCount -= solveline(puzzle, Pop(stack[COL]), stack[ROW], COL);
		unsolvedCellCount -= solveline(puzzle, Pop(stack[ROW]), stack[COL], ROW);
	}

	printf("\n\n\n");
	ExportSolution(puzzle);
	printf("\n\n\n");
	if (unsolvedCellCount > 0) {
/*		Puzzle* newPuzzle = CopyPuzzle(puzzle);

		int row = 0, col = 0;
		//get a random unknown cell, store coords		
		
		//set it to STATE_FULL		
		Push(stack[ROW], (void*) &newPuzzle->line[ROW][row]);
		Push(stack[COL], (void*) &newPuzzle->line[COL][col]);
		solve(newPuzzle, stack, unsolvedCellCount);
		
		//set it to STATE_BLNK
		Push(stack[ROW], (void*) &newPuzzle->line[ROW][row]);
		Push(stack[COL], (void*) &newPuzzle->line[COL][col]);
		solve(newPuzzle, stack, unsolvedCellCount);*/
	} else
	if (unsolvedCellCount == 0) {
		ClearStack(stack[ROW]);	//could just not push perpendicular lines that are already
		ClearStack(stack[COL]);	//solved instead of clearing stacks here

		ExportSolution(puzzle);
	} else {
		//invalid solution, get out
	}
	
	//free puzzle memory here
	
	return;
}

int main (int n, char** args) {
	if (n < 2) errorout(ERROR_ARGS, "No file name was given.");
	
	Puzzle* puzzle = getPuzzle(args[1]);
	
	Stack** stack = (Stack**) malloc(AXES*sizeof(Stack*));
	stack[ROW] = CreateStack();
	stack[COL] = CreateStack();
	
	int i;
	for (i = 0; i < puzzle->length[ROW]; i++)	Push(stack[ROW], (void*) &puzzle->line[ROW][i]);
	for (i = 0; i < puzzle->length[COL]; i++)	Push(stack[COL], (void*) &puzzle->line[COL][i]);
	
	solve(puzzle, stack, puzzle->length[ROW] * puzzle->length[COL]);
	
	ExportSolution(NULL);
	
	return 0;	
}
