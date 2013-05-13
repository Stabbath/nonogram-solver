#include <stdio.h>
#include <stdlib.h>
#include "stacks.h"
#include "solver.h"


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
  *	@return int :							sum; 															*
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
//	free(puzzle->name);
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

	Stack** stack = (Stack**) malloc((AXES+1)*sizeof(Stack*));
	stack[ROW] = CreateStack();
	for (i = 0; i < puzzle->length[ROW]; i++)	Push(stack[ROW], (void*) &puzzle->line[ROW][i]);	//O(Lr)
	stack[COL] = CreateStack();
	for (i = 0; i < puzzle->length[COL]; i++)	Push(stack[COL], (void*) &puzzle->line[COL][i]);	//O(Lc)

	return stack;
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * PickCell: O(L²)		Gets the size of the largest block in a line									*
  * 																									*
  * @param Line* :		line to get largest size from													*
  *	@return int :		length of largest block															*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Cell* PickCell(Puzzle* puzzle, int *row, int *col) {	//worst case: O(Lr*Lc) = O(L²)		, average is much much lower though
	int i, j;						//TODO still, L² is pretty bad.. should think about this
	for (i = 0; i < puzzle->length[ROW]; i++) {	//O(Lr*Lc)
		for (j = 0; j < puzzle->length[COL]; j++) {	//O(Lc)
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

