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

void FreePuzzle(Puzzle* puzzle) {

}

//finish this function
Puzzle* CopyPuzzle(Puzzle* old) {
	Puzzle* puzzle = (Puzzle*) malloc(sizeof(Puzzle));
	
	puzzle->length = (int*) malloc(AXES*sizeof(int));
	puzzle->length[ROW] = old->length[ROW];
	puzzle->length[COL] = old->length[COL];
	
	puzzle->line = (Line**) malloc(AXES*sizeof(Line*));
	puzzle->line[ROW] = (Line*) malloc(puzzle->length[ROW]*sizeof(Line));
	puzzle->line[COL] = (Line*) malloc(puzzle->length[COL]*sizeof(Line));

	int i, j, k;
	k = ROW;
	for (i = 0; i < puzzle->length[k]; i++) {
		puzzle->line[k][i].blockNum = old->line[k][i].blockNum;
		puzzle->line[k][i].block = (Block*) malloc(puzzle->line[k][i].blockNum*sizeof(Block));
		for (j = 0; j < puzzle->line[k][i].blockNum; j++) {
			puzzle->line[k][i].block[j].length = old->line[k][i].block[j].length;
		}

		puzzle->line[k][i].cells = (Cell**) malloc(puzzle->length[k]*sizeof(Cell*));
		for (j = 0; j < puzzle->length[k]; j++) {
			puzzle->line[k][i].cells[j] = (Cell*) malloc(sizeof(Cell));
			puzzle->line[k][i].cells[j]->state = old->line[k][i].cells[j]->state;
		}
	}
	k = COL;
	for (i = 0; i < puzzle->length[COL]; i++) {
		puzzle->line[k][i].blockNum = old->line[k][i].blockNum;
		puzzle->line[k][i].block = (Block*) malloc(puzzle->line[k][i].blockNum*sizeof(Block));
		for (j = 0; j < puzzle->line[k][i].blockNum; j++) {
			puzzle->line[k][i].block[j].length = old->line[k][i].block[j].length;
		}

		puzzle->line[k][i].cells = (Cell**) malloc(puzzle->length[k]*sizeof(Cell*));
		for (j = 0; j < puzzle->length[k]; j++) {
			puzzle->line[k][i].cells[j] = puzzle->line[k][j].cells[i];
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
		return puzzle->length[ROW] * puzzle->length[COL];
	}
	
	int i, n;
	if (sum == 0) {	//means the whole line has blanks (special case)
		for (i = 0, n = 0; i < puzzle->length[coord]; i++) {
			if (line->cells[i]->state == STATE_UNKN) {
				line->cells[i]->state = STATE_BLNK;
				n++;
			}
			return n;
		}
	} else
	if (/*puzzle->length[coord] == sum*/1) {	//means the right-most arrangement is the same as the left-most: full solution is known (special case)
//		printf("\t\tnot 0\n");//go through block lengths
	} //else - commented since there's no code for the special case yet*/
	if (puzzle->length[coord] - sum < cap) {	//only situation in which stacking will do anything
		Line* left = (Line*) malloc(sizeof(Line));
		Line* right = (Line*) malloc(sizeof(Line));
		LineCopy(left, line, puzzle->length[coord]);
		LineCopy(right, line, puzzle->length[coord]);
		
//		int opCoord = (coord == ROW ? COL : ROW);
		for (i = 0, n = 0; i < puzzle->length[coord]; i++) {
			printf("%c vs %c", left->cells[i]->state, right->cells[i]->state);
			if (left->cells[i]->state == right->cells[i]->state) {
				if (line->cells[i]->state == STATE_UNKN) {
					line->cells[i]->state = left->cells[i]->state;
					n++;
	//				Push(stack, (void*) &puzzle->line[opCoord][i]);
				}
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

Cell* PickCell(Puzzle* puzzle) {
	int i, j;
	for (i = 0; i < puzzle->length[ROW] ; i++) {
		for (j = 0; j < puzzle->length[COL] ; j++) {
			if (puzzle->line[ROW][i].cells[j]->state == STATE_UNKN) {
				return puzzle->line[ROW][i].cells[j];
			}
		}
	}
	return NULL;
}

void solve(Puzzle* puzzle, Stack** stack, int unsolvedCellCount) {
	while ((!IsStackEmpty(stack[ROW]) || !IsStackEmpty(stack[COL])) && unsolvedCellCount > 0) {
		printf("unsolvedcellcount: %d\n", unsolvedCellCount);
		unsolvedCellCount -= solveline(puzzle, Pop(stack[ROW]), stack[ROW], ROW);
		unsolvedCellCount -= solveline(puzzle, Pop(stack[COL]), stack[COL], COL);
		printf("\n\n");
		ExportSolution(puzzle);
		printf("\n\n");
	}
	
	printf("unsolvedcellcount: %d\n", unsolvedCellCount);
	printf("out of loop!\n");	
	
	if (unsolvedCellCount > 0) {
		Puzzle* newPuzzle;

		int row = 0, col = 0;
		Cell* pick;	//note: the way this is currently implemented, there can be no "random" elements in the picking of cells: the same puzzle must always lead to the same cell getting picked
		
		newPuzzle = CopyPuzzle(puzzle);
		(pick = PickCell(newPuzzle))->state = STATE_FULL;
		Push(stack[ROW], (void*) &newPuzzle->line[ROW][row]);
		Push(stack[COL], (void*) &newPuzzle->line[COL][col]);
		solve(newPuzzle, stack, unsolvedCellCount);
		FreePuzzle(newPuzzle);
			
		newPuzzle = CopyPuzzle(puzzle);
		(pick = PickCell(newPuzzle))->state = STATE_BLNK;
		Push(stack[ROW], (void*) &newPuzzle->line[ROW][row]);
		Push(stack[COL], (void*) &newPuzzle->line[COL][col]);
		solve(newPuzzle, stack, unsolvedCellCount);
		FreePuzzle(newPuzzle);
	} else
	if (unsolvedCellCount == 0) {
		ClearStack(stack[ROW]);	//could just not push perpendicular lines that are already
		ClearStack(stack[COL]);	//solved instead of clearing stacks here

		ExportSolution(puzzle);
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
	
	ExportSolution(NULL);
	
	return 0;	
}
