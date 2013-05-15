#include <stdio.h>
#include <stdlib.h>
#include "stacks.h"
#include "solver.h"


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getSumOfBlocks: O(N)			adds up the lengths of every block							 		*
  *																										*
  * @param Line* :					line to get sum for													*
  *	@return int : 					sum of lengths														*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int getSumOfBlocks(Line* line) {
	if (line->block[0].length == 0) return 0;
	
	int i, count = 0;
	for (i = 0; i < line->blockNum; i++) {
		count += line->block[i].length;
	}
	return count;
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * checkline: O(L)					checks if a line layout is possible							 		*
  *																										*
  * @param Line* :					line to test														*
  * @param int :					number of cells in this line										*
  *	@return int:					1 if possible, 0 if not												*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int checkline(Line* line, int length) {
	if (line->block[0].length == 0) return 1;	//blank lines are correctly always filled

	int sum = getSumOfBlocks(line);
	int count = 0;
	int streak = 0;
	int j;
	Stack* streakStack = CreateStack();
	for (j = 0; j < length; j++) {
		if (line->cells[j]->state == STATE_FULL) {
			count++;
			streak++;
		}
		if (line->cells[j]->state == STATE_BLNK || j == length - 1) {
			if (streak != 0) {
				int* tmp = malloc(sizeof(tmp));
				*tmp = streak;
				Push(streakStack, (void*) tmp);
			}
			streak = 0;
		}
	}

	int success = 1;

	if (sum != count) {	//number of # is different from what was expected
		success = 0;
	}	
	
	if (success) {
		int n;
		int* tmp;
		for (n = line->blockNum - 1; n >= 0; n--) {
			if (IsStackEmpty(streakStack)) {	//means there were less blocks than there were supposed to be
				success = 0;
				break;
			}
		
			tmp = (int*) Pop(streakStack);
			if (line->block[n].length != *tmp) {	//means this block had the wrong length
				success = 0;
			}
		
			free(tmp);
		}
	}
	if (!IsStackEmpty(streakStack)) {
		success = 0;	//means there were more blocks than there were supposed to be
		while (!IsStackEmpty(streakStack)) {
			free(Pop(streakStack));
		}
	}
	free(streakStack);
	
	return success;
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * checkpuzzle: O(L²)				checks if a puzzle layout is a possible solution			 		*
  *																										*
  * @param Puzzle* :				puzzle that was solved												*
  *	@return int :					1 if possible, 0 if not												*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int checkpuzzle(Puzzle* puzzle) {
	int i, x;
	Line* lines;
	for (x = ROW; x < AXES; x++) {
		lines = puzzle->line[x];
		for (i = 0; i < puzzle->length[x]; i++) {	//check every row and column
			if (!checkline(&lines[i], puzzle->length[!x])) return 0;
		}
	}
	
	return 1;
}




/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * LinkCellsToLines: O(L²)			macro function. stores the pointers to a cell's row and column 		*
  *									inside of the cell													*
  *																										*
  * @param Puzzle* :				puzzle that's being solved											*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LinkCellsToLines(Puzzle* puzzle) {
	int i, j;	//store lines each cell belongs to inside of the cell structs
	for (i = 0; i < puzzle->length[ROW]; i++) {
		for (j = 0; j < puzzle->length[COL]; j++) {
			puzzle->line[ROW][i].cells[j]->row = &puzzle->line[ROW][i];
			puzzle->line[COL][j].cells[i]->col = &puzzle->line[COL][j];
		}
	}
}




/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * SetupMinsAndMaxes: O(L*N)		macro function. finds the lowest and highest indexes that may belong*
  *									to each block														*
  *																										*
  * @param Puzzle* :				puzzle that's being solved											*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SetupMinsAndMaxes(Puzzle* puzzle) {
	/*get basic mins and maxes for each block*/
	int i, j, n, min, max, length;
	Line* line;
	for (j = ROW; j < AXES; j++) {
		for (i = 0; i < puzzle->length[j]; i++) {
			line = &puzzle->line[j][i];
			length = puzzle->length[!j];
			min = 0;	//min for first block is 0
			max = length - 1 - (getMinSumOfBlocksAndBlanks(line, 0) - line->block[0].length);	//max is this
			for (n = 0; n < line->blockNum; n++) {
				line->block[n].min = min;
				line->block[n].max = max;
				
				if (n < line->blockNum - 1) {	//everytime we loop, the min and max are offset by the blocks' sizes
					min += line->block[n].length + 1;
					max += line->block[n + 1].length + 1;
				}
			}
		}
	}
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getMinSumOfBlocksAndBlanks:	O(N)	Adds up the sizes of all blocks AFTER the current one, and then *
  * 									adds the mandatory amount of blanks. Counts the current block.  *
  *																										*
  * @param Line* :						line to get sum of												*
  * @param int :						block index to start on											*
  *	@return int :						sum; will return a negative number if n is out of bounds		*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int getMinSumOfBlocksAndBlanks(Line* line, int n) {
	int i, count = 0;
	if (line->block[0].length == 0) return 0;
	
	for (i = n; i < line->blockNum; i++) {	//O(N - n) = O(N) worst case
		count = count + line->block[i].length;
	}
	return count + (line->blockNum - n - 1);	//mandatory spaces, 1 for every block after n
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getMinSumOfBlocksAndBlanksPrev:	O(N)	Adds up the sizes of all blocks BEFORE the current one, and then*
  * 										adds the mandatory amount of blanks.							*
  *																											*
  * @param Line* :							line to get sum of												*
  * @param int :							block index to start on											*
  *	@return int :							sum 															*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int getMinSumOfBlocksAndBlanksPrev(Line* line, int n) {
	if (n == 0) return 0;
	
	int i, count = 0;
	for (i = n - 1; i >= 0; i--) {	//O(N - n) = O(N) worst case
		count += line->block[i].length;
	}
	return count + n;	//mandatory spaces, 1 for every block before n (which is = n)
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getLengthOfLargestBlock: O(N)		Gets the size of the largest block in a line					*
  * 																									*
  * @param Line* :						line to get largest size from									*
  *	@return int :						length of largest block											*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int getLengthOfLargestBlock(Line* line) {
	int i, size;
	for (i = 0, size = 0; i < line->blockNum; i++) {
		if (line->block[i].length > size)	size = line->block[i].length;
	}
	return size;
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * FreePuzzle: O(L²)		Just a macro function to clean up main. Frees memory allocated to puzzle.	*
  *																										*
  * @param Puzzle* :		puzzle to destroy															*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FreePuzzle(Puzzle* puzzle) {	//O(Lr*Lc) + O(Lc) = O((Lr+1)*Lc) = O(Lr*Lc) = O(L²)
	int i, j;
	
	for (i = 0; i < puzzle->length[ROW]; i++){	//O(Lr)
		for (j = 0; j < puzzle->length[COL]; j++){	//O(Lc)
			free(puzzle->line[ROW][i].cells[j]);
		}
		free(puzzle->line[ROW][i].cells);
		free(puzzle->line[ROW][i].block);
	}
	for (i = 0; i < puzzle->length[COL]; i++){	//O(Lc)
		free(puzzle->line[COL][i].cells);
		free(puzzle->line[COL][i].block);
	}
	
	free(puzzle->line[ROW]);
	free(puzzle->line[COL]);
	free(puzzle->line);
	free(puzzle->length);
	free(puzzle);
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * FreeStacks: O(1)		Just a macro function to clean up main. Frees memory allocated to stacks.	*
  *																										*
  * @param Stack** :		stack array to destroy														*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FreeStacks(Stack** stack) {
	ClearStack(stack[ROW]);
	ClearStack(stack[COL]);
	free(stack[ROW]);
	free(stack[COL]);
	free(stack);
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * InitStacks: O(L)		Just a macro function to clean up main. Allocates memory to stacks and 		*
  *							pushes every row and column to its respective stack.						*
  *																										*
  * @param Puzzle* :		puzzle to get ROW|COL lengths from											*
  *	@return	Stack** :		stack array that was created and initialised								*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Stack** InitStacks(Puzzle* puzzle) {	//O(2L)	= O(L)
	int i;

	Stack** stack = (Stack**) malloc(AXES*sizeof(Stack*));
	stack[ROW] = CreateStack();
	for (i = 0; i < puzzle->length[ROW]; i++)	Push(stack[ROW], (void*) &puzzle->line[ROW][i]);	//O(Lr)
	stack[COL] = CreateStack();
	for (i = 0; i < puzzle->length[COL]; i++)	Push(stack[COL], (void*) &puzzle->line[COL][i]);	//O(Lc)

	return stack;
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * PickCell: O(L²)		Loops through puzzle cells and returns the first unidentified cell				*
  * 																									*
  * @param Puzzle* :	puzzle to find cell in															*
  * @param int* :		int ptr for storing the cell's row's index										*
  * @param int* :		int ptr for storing the cell's column's index									*
  *	@return Cell* :		picked cell																		*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Cell* PickCell(Puzzle* puzzle, int *row, int *col) {	//worst case: O(Lr*Lc) = O(L²)		, average is much much lower though
	int i, j;
	for (i = 0; i < puzzle->length[ROW]; i++) {
		for (j = 0; j < puzzle->length[COL]; j++) {
			if (puzzle->line[ROW][i].cells[j]->state == STATE_UNKN) {
				*row = i;
				*col = j;
				return puzzle->line[ROW][i].cells[j];
			}
		}
	}
	return NULL;
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * ConditionalPush: O(1)		Controls which lines are allowed to be pushed onto the stack, to improve*
  * 							performance.															*
  *																										*
  * @param Stack* :		stack we're pushing to															*
  * @param Line* :		line we're pushing																*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ConditionalPush(Stack* stack, Line* line) {
	if (line->unsolvedCells > 0) {
		Push(stack, (void*) line);
	}
}

