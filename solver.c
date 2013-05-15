#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stacks.h"
#include "solver.h"

#define MODE_GET 0
#define MODE_RESET 1
#define MODE_TEST 2

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * MergeBlockPositions: 	Tests a line's different block configurations against each other as they are*
  *	O(L)					identified by ExamineBlocks. Finds out which cells can be determined with 	*
  *							certainty.																	*
  *																										*
  * @param Line* :			line we're merging with the current solution								*
  * @param int :			number of cells in this line (as well as in the solution)					*
  * @param int :			MODE_GET|MODE_RESET|MODE_TEST - operating mode								*
  *	@return :				solution to line after block mergers										*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Line* MergeBlockPositions(Line* line, int length, int mode) {	//O(L)
	static Line* solution = NULL;
	int i;
	
	switch (mode) {
		/* reset: clear solution */
		case MODE_RESET: {
			for (i = 0; i < length; i++) {
				free(solution->cells[i]);
			}
			free(solution->cells);
			free(solution->block);
			free(solution);
			solution = NULL;
			break;
		}
		/* get: just GET out of here and return solution */
		case MODE_GET: {
			break;
		}
		/* compare line arg with current solution. If no current solution, create solution line and copy the arg line to it */
		case MODE_TEST: {
			if (solution == NULL) {		//line is brand new, we had no previous solution
				/* clone the line */
				solution = (Line*) malloc(sizeof(Line));
				solution->blockNum = line->blockNum;
				solution->block = (Block*) malloc(solution->blockNum*sizeof(Block));
				for (i = 0; i < solution->blockNum; i++) {
					solution->block[i].length = line->block[i].length;
					solution->block[i].min = line->block[i].min;
					solution->block[i].max = line->block[i].max;
				}

				solution->cells = (Cell**) malloc(length*sizeof(Cell*));
				for (i = 0; i < length; i++) {
					solution->cells[i] = (Cell*) malloc(sizeof(Cell));
					solution->cells[i]->state = line->cells[i]->state;
				}
			
			} else {	//if it's not the first time, then we have to update the solution based on mismatches with the new version of the line
				/* every cell in this version of the line that mismatches the previously held solution gets set to unknown */
				for (i = 0; i < length; i++) {
					if (line->cells[i]->state != solution->cells[i]->state) {
						solution->cells[i]->state = STATE_UNKN;
					}
				}
			}
			break;
		}
	}
	
	return solution;
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * ExamineBlocks:			Tests every possible block position based on previous block positions, and	*
  *	O(TODO)					attempts to discover implicit cells.										*
  *																										*
  * @param Line* :			line whose blocks we're examining											*
  * @param int :			block index we're on														*
  * @param int :			length of line																*
  * @param int :			first cell of the current test position for this block (inclusive)			*
  * @param int :			last cell of the current test position for this block (inclusive)			*
  *	@noreturn :																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ExamineBlocks(Line* line, int n, int length, int start, Stack* cellstack, int prevblockend) {
	int i, count = 0;
	Cell** cells = line->cells;
	
	/* fill blanks before position until the prev block's ending index*/
	if (start > 0) {	//beginning blank
		if (cells[start-1]->state == STATE_FULL) return;
		if (cells[start-1]->state == STATE_UNKN) {
			cells[start-1]->state = STATE_BLNK;
			Push(cellstack, cells[start-1]);
			count++;
		}
		for (i = start - 2; i > prevblockend; i--) {		//fill blanks between this block and previous one
			if (cells[i]->state == STATE_UNKN) {
				cells[i]->state = STATE_BLNK;
				Push(cellstack, cells[i]);
				count++;
			} else
			if (cells[i]->state == STATE_FULL) {
				while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;
				return;
			}
		}
	}

	/* fill this block's current position's cells */
	for (i = start; i <= start + line->block[n].length - 1; i++) {
		if (cells[i]->state == STATE_BLNK) {
			while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;
			return;
		}
		if (cells[i]->state == STATE_UNKN) {
			cells[i]->state = STATE_FULL;
			Push(cellstack, line->cells[i]);
			count++;
		}
	}

	/* fill blanks after position */	
	if (i < length) {	//terminating blank
		if (cells[i]->state == STATE_FULL) {
			while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;
			return;
		}
		if (cells[i]->state == STATE_UNKN) {
			cells[i]->state = STATE_BLNK;
			Push(cellstack, cells[i]);
			count++;
		}
		if (n == line->blockNum - 1) {
			for (/*dont change i*/; i < length; i++) {		//fill blanks between this block and the end, since it's the last block
				if (cells[i]->state == STATE_UNKN) {
					cells[i]->state = STATE_BLNK;
					Push(cellstack, cells[i]);
					count++;
				} else
				if (cells[i]->state == STATE_FULL) {
					while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;
					return;
				}
			}
		}
	}
	
	if (n < line->blockNum - 1) {	//not all blocks are in a position yet, go deeper
		/* test every possible position of the remaining blocks */
		int j;
		int min = line->block[n+1].min;
		int max = line->block[n+1].max;
		int size = line->block[n+1].length;
		for (j = min; j <= max - size + 1; j++) {	//test filling blocksize cells after i = min for every possible block start
			ExamineBlocks(line, n + 1, length, j, cellstack, i  - 1);
		}
	} else {	//all blocks are in a position, time to test them
		MergeBlockPositions(line, length, MODE_TEST);
	}
	
	/* undo changes made to cells */
	while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;	
	return;
}




#define IMPOSSIBLE		puzzle->length[ROW]*puzzle->length[COL]
/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * solveline:				Solves cells based on the line's native characteristics, ie block number 	*
  * O(TODO)					and block sizes, COUPLED WITH all information that's already been gathered	*
  *							regarding identified cells.													*
  *																										*
  * @param Puzzle* :		puzzle we're working on														*
  * @param Line* :			line to solve																*
  * @param Stack** :		support stack array															*
  * @param int :			ROW|COL - coordinate of this line											*
  *	@return int :			number of solved cells, or -1 if an impossibility was discovered.			*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int solveline(Puzzle* puzzle, Stack** stack, Stack* cellstack, int x) {
	Line* line = Pop(stack[x]);
//	if (line->unsolvedCells == 0) return 0;		//don't test solved lines
	
	int length = puzzle->length[!x];
	int solvedCells = 0;
	int i;
	
	/* start recursive analysis of block positions */
	int highestMin = line->block[0].max - line->block[0].length + 1;
	for (i = line->block[0].min; i <= highestMin; i++) {	//test filling blocksize cells after i = min for every possible block start
		Stack* st = CreateStack();
		ExamineBlocks(line, 0, length, i, st, -1);
		while (!IsStackEmpty(st)) ((Cell*) Pop(st))->state = STATE_UNKN;	//reset just in case
		free(st);
	}
	
	
	Line* solution = MergeBlockPositions(NULL, length, MODE_GET);
	if (solution == NULL) return IMPOSSIBLE;	//NULL means we didn't succeed at all in the previous loop
	for (i = 0; i < length; i++) {
		if (line->cells[i]->state != solution->cells[i]->state) {
			if (line->cells[i]->state == STATE_UNKN) {
				line->cells[i]->state = solution->cells[i]->state;
				line->cells[i]->row->unsolvedCells--;
				line->cells[i]->col->unsolvedCells--;
				solvedCells++;
				Push(cellstack,line->cells[i]);
				ConditionalPush(stack[!x],&puzzle->line[!x][i]);
			} else {
				MergeBlockPositions(NULL, length, MODE_RESET);
				return IMPOSSIBLE;	//can this even get this far without detection? better safe than sorry though!
			}
		}
	}
	
	
	MergeBlockPositions(NULL, length, MODE_RESET);
	return solvedCells;
}
#undef IMPOSSIBLE




/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * solve: O(TODO)			Solves cells in a complete way to find all possible solutions to the puzzle	*
  *																										*
  * @param Puzzle* :		puzzle to solve																*
  * @param Stack** :		stack array	to push rows and columns to										*
  * @param Stack* :			stack to push changed cells to												*
  * @param int :			number of unsolved cells													*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void solve(Puzzle* puzzle, Stack** stack, Stack* cellstack, int unsolvedCellCount) {
	while (!(IsStackEmpty(stack[ROW]) && IsStackEmpty(stack[COL])) && unsolvedCellCount > 0) {
		if (!IsStackEmpty(stack[ROW])) {
			unsolvedCellCount -= solveline(puzzle, stack, cellstack, ROW);
		}
		if (!IsStackEmpty(stack[COL])) {
			unsolvedCellCount -= solveline(puzzle, stack, cellstack, COL);
		}
	}

	if (unsolvedCellCount > 0) {	//puzzle could not be fully solved through regular means... time to guess
		Stack* nextcellstack = CreateStack();	//for reverting changes in case guess is wrong
		
		int row, col;
		Cell* pick = PickCell(puzzle, &row, &col);	//cell we're going to guess values
		Cell* cell;
		
		/* even if the cell is wrong, it has to be considered correct */
		puzzle->line[ROW][row].unsolvedCells--;
		puzzle->line[COL][col].unsolvedCells--;
		
		/* try the value # and then try solving again based on the new information */
		pick->state = STATE_FULL;
		Push(stack[ROW], (void*) &puzzle->line[ROW][row]);
		Push(stack[COL], (void*) &puzzle->line[COL][col]);
		solve(puzzle, stack, nextcellstack, unsolvedCellCount - 1);
		while (!IsStackEmpty(nextcellstack)) {	//undo changes
			cell = (Cell*) Pop(nextcellstack);
			cell->state = STATE_UNKN;
			cell->row->unsolvedCells++;
			cell->col->unsolvedCells++;
		}
		
		/* try value - and then try solving again based on the new information. 
		   even if # led to a solution, this will let us find ALL solutions */
		pick->state = STATE_BLNK;
		Push(stack[ROW], (void*) &puzzle->line[ROW][row]);
		Push(stack[COL], (void*) &puzzle->line[COL][col]);
		solve(puzzle, stack, nextcellstack, unsolvedCellCount - 1);
		while (!IsStackEmpty(nextcellstack)) {	//undo changes
			cell = (Cell*) Pop(nextcellstack);
			cell->state = STATE_UNKN;
			cell->row->unsolvedCells++;
			cell->col->unsolvedCells++;
		}
		
		pick->state = STATE_UNKN;	//undo the picked cell
		puzzle->line[ROW][row].unsolvedCells++;
		puzzle->line[COL][col].unsolvedCells++;
		
		ClearStack(nextcellstack);	//should already be cleared, but clear it anyway
		free(nextcellstack);
	} else
	if (unsolvedCellCount == 0) {	//the puzzle has no more '?'s
		if (checkpuzzle(puzzle)) ExportSolution(puzzle, NULL);	//check solution and export it if correct
	} else {
		//invalid solution, get out
	}
	
	ClearStack(stack[ROW]);	//in case they are not empty yet
	ClearStack(stack[COL]);
	
	return;
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * main: O(TODO)			Solves a nonogram puzzle													*
  *																										*
  * @param int :			number of arguments															*
  * @param char** :			string array of arguments													*
  *	@return int :			always returns 0 (or exit(1) if an error occurs)							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(int num, char** args) {
	if (num < 2) errorout(ERROR_ARGS, "No file name was given.");
	
	if (strlen(args[1]) >= MAXPATH) errorout(ERROR_ARGS, "Filename too long.");
	
	Puzzle* puzzle = getPuzzle(args[1]);	//O(L²)
		
	int unsolvedCellCount = presolve(puzzle);
	if (unsolvedCellCount > 0) {	//O(L*N²)
		LinkCellsToLines(puzzle);
		SetupMinsAndMaxes(puzzle);

		Stack** stack = InitStacks(puzzle);	//O(L)
		solve(puzzle, stack, NULL, unsolvedCellCount);		//O(TODO)
		FreeStacks(stack);	//O(1)
	} else
	if (unsolvedCellCount == 0) {
		ExportSolution(puzzle, NULL);
	}
	
	ExportSolution(NULL, puzzle->name);	//O(1)
	
	FreePuzzle(puzzle);	//O(L²)
	
	return 0;
}
