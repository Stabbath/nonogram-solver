#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stacks.h"
#include "solver.h"

 /* * * * * * * * * * * * * * * * * * * * * *
  * Complexity variables:					*
  *	N - number of blocks in line			*
  *	L - length of line (r|c => row|column)	*
  *	B - length of block						*
  * S - sum of blocks + blanks in line		*
  * * * * * * * * * * * * * * * * * * * * * */


//NOTE: these functions can be merged, with 'blockid' being incremented based on which check
//we want. Would also allow for checks with blockid + n, for any n
//also, there is no need for the 'i' var

/*can all remaining blocks and spaces (including the current block)
  fit after the current cell?*/
int checkA(Line* line, int blockid, int unkns, int index, int length) {
	int i, count = 0;
	for (i = blockid; i < line->blockNum; i++) count += 1 + line->block[i].length;
	count--; //because we considered 1 space too many
	
	if (count < (length - 1) - index) {	//means we can't fit all the blocks and spaces
		return 0;
	}
	return 1;
}

/*can all remaining blocks and spaces (EXCLUDING the current block)
  fit after the current cell?*/
int checkAtilde(Line* line, int blockid, int unkns, int index, int length) {
	int i, count = 0;
	for (i = blockid + 1; i < line->blockNum; i++) count += 1 + line->block[i].length;
	count--; //because we considered 1 space too many
	
	if (count < (length - 1) - index) {	//means we can't fit all the blocks and spaces
		return 0;
	}
	return 1;
}


#define NextBlock curBlock++;unknCount=0;fullCount=0;

void WIP_solveline(Puzzle* puzzle, Line* line, int x) {
	int curBlock = 0;
	int solvedCells = 0;
	int unknCount = 0;
	int fullCount = 0;
	int i;
	
	for (i = 0; i < puzzle->length[x]; i++) {	//we're going to go through every cell in the line
		if (line->cells[i]->state == STATE_FULL) { //== '#' ?
			fullCount++;
			if (unknCount == 0) {
				for (;line->cells[i]->state == STATE_FULL; i++) {
					fullCount++;
					if (fullCount > line->block[curBlock].length) {
						//impossible
					}
				}
				
				if (line->cells[i]->state == STATE_BLNK) {	//ended by a '-' ?
					if (fullCount == line->block[curBlock].length) {
						NextBlock
						continue;
					} else {
						//impossible
					}
				} else {	//ended by a '?' ?
					if (fullCount == line->block[curBlock].length) {	//append '-' and finish block
						line->cells[i]->state = STATE_BLNK;
						solvedCells++;
					} else {
						while (fullCount != line->block[curBlock].length) {	//finish the block
							if (line->cells[i]->state == STATE_FULL) {
								fullCount++;
							} else 
							if (line->cells[i]->state == STATE_BLNK) {
								//impossible
							} else {
								fullCount++;
								solvedCells++;
								line->cells[i]->state = STATE_FULL;
							}
							i++;
						}
					}
					NextBlock
					continue;
				}		
			} else {
				//not sure yet, complex shit
				//
				//
			}
		} else
		if (line->cells[i]->state == STATE_BLNK) {	//== '-' ?
			if (unknCount == 0) { //passed cells == 0?
				
			} else {
				if (unknCount < line->block[curBlock].length) {	//passed cells < block size?
					if (checkA(line, curBlock, unknCount, i, puzzle->length[x])) {
						//read cells backwards as long as they're unknown, set to BLNK, then goto readcell
						continue;	//goto next cell
					} else {
						//impossible
						break;
					}
				} else {
					if (checkA(line, curBlock, unknCount, i, puzzle->length[x])) {
						//not sure yet, complex shit
						//
						//
					} else {
						if (checkAtilde(line, curBlock, unknCount, i, puzzle->length[x])) {
							//check difference between passed cells and block length,
							//stack the block and fill a few #'s
							//NOTE: if passed cells >> blocksize, we probably have several blocks in this
							//space, need to do check to see which blocks can/must fit in this space, which
							//can/must fit afterward, and stack them
							NextBlock
							continue;
						} else {
							//impossible
							break;
						}
					}
				}
			}
		} else {	//== '?' ?
			unknCount++;
			//careful if passed cells >> block length
			continue;
		}
	}
}

//note: check to make sure that sum of blocks + blanks is <= line length in getPuzzle: if a line is not, just quit since puzzle is impossible


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * FreePuzzle: O(Lr*Lc)	- Just a macro function to clean up main. Frees memory allocated to puzzle. *
  *																										*
  * @param Puzzle* :		puzzle to destroy															*
  *	@noreturn																							*
  * 																									*
  * @verbose : 				nothing to be verbose about													*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FreePuzzle(Puzzle* puzzle) {	//O(Lr*Lc) + O(Lc) = O((Lr+1)*Lc) = O(Lr*Lc)
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



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getMinSumOfBlocksAndBlanks:	O(N)	- Adds up the sizes of all blocks, and then adds (blocknum - 1)	*
  * 									for the mandatory amount of blanks.								*
  *																										*
  * @param Line* :		line to get sum of																*
  *	@return int :		sum																				*
  * 																									*
  * @verbose : Nothing to be verbose about.																*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int getMinSumOfBlocksAndBlanks(Line* line) {
	int i, count;
	for (i = 0, count = 0; i < line->blockNum; i++) {
		count += line->block[i].length;
	}
	count += 1*(line->blockNum - 1);

	return count;
}




/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getLengthOfLargestBlock:	O(N)	- Gets the size of the largest block in a line					*
  * 																									*
  * @param Line* :		line to get largest size from													*
  *	@return int :		length of largest block															*
  * 																									*
  * @verbose : Nothing to be verbose about.																*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int getLengthOfLargestBlock(Line* line) {
	int i, size;
	for (i = 0, size = 0; i < line->blockNum; i++) {
		if (line->block[i].length > size)	size = line->block[i].length;
	}
	return size;
}




/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * stackline:					O(N)	- Solves cells implicitly based on the line's native 			*
  * 									characteristics, ie block number and block sizes.				*
  *																										*
  * NOTE: this routine will never provide new information if used more than once for a single line.		*
  *																										*
  * @param Line* :			line to solve																*
  * @param Stack* :																						*
  * @param int :			coord that this line 														*
  *	@return int :			number of solved cells, or -1 if an impossibility was discovered.			*
  * 																									*
  * @verbose : TODO																						*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define IMPOSSIBLE	-1
int stackline(Line* line, int length) {	//O(N) + max((O(L), O(N*B), O(N*(B-L+S))) TODO
	int sum = getMinSumOfBlocksAndBlanks(line);	//O(N)
	int cap = getLengthOfLargestBlock(line);	//O(N)
	
	if (sum > length) {
		return IMPOSSIBLE;	//impossible
	}
	
	int i, ret = 0;
	if (sum == 0) {	//O(L)
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 * Special case: If sum is 0, then the whole line is blank!  *
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		for (i = 0; i < length; i++) {	//O(L)
			if (line->cells[i]->state == STATE_UNKN) {
				line->cells[i]->state = STATE_BLNK;
				ret++;
			} else 
			if (line->cells[i]->state == STATE_FULL) {
				return IMPOSSIBLE;	//impossible
			}
			return ret;
		}
	} else
	if (length == sum) {	//O(N*B)
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 * Special case: If sum is length, then the whole line is known! *
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		int i, j, l = 0;
		for (i = 0; i < line->blockNum; i++, l++) {	//O(N)
			for (j = 0; j < line->block[i].length; j++, l++) {	//O(B)
				if (line->cells[l]->state == STATE_UNKN) {
					line->cells[l]->state = STATE_FULL;
					ret++;
				} else
				if (line->cells[l]->state == STATE_BLNK) {
					return IMPOSSIBLE;	//impossible
				}
			}
			
			if (l < length) {
				if (line->cells[l]->state == STATE_UNKN) {
					line->cells[l]->state = STATE_BLNK;
					ret++;
				} else
				if (line->cells[l]->state == STATE_FULL) {	//next cell must be blnk
					return IMPOSSIBLE;		//impossible
				}
			}
		}
		return ret;
	} else
	if (length - sum < cap) {	//O(N*(B-L+S))				//only situation in which stacking will do anything
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 * Using simple math we can identify which cells must be filled and which remain unknown. The first  *
		 * length-sum cells are unaffected because length-sum is the difference between the first cell of the*
		 * left-most and right-most positions of the any block in the line. Then, blocksize-(length-sum) is  *
		 * the amount of cells which must be filled if ANY cells at all are to be filled (ie if there is a   *
		 * superposition of the leftmost and rightmost possibilities). If no cells can be filled, we skip    *
		 * blocksize cells to account for that block's placement, otherwise we skip another length-sum to the*
		 * right of the block. Also, if length - sum < cap, we know there is no superposition for any block. *
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		int n = 0;
		i = length - sum;												//the first (length - sum) cells are unaffected, skip them
		while (n < line->blockNum) {	//O(N)
			int limit = i + line->block[n].length - (length - sum);		//the next (blocksize - (linelength - sum)) cells are full
			if (line->block[n].length > length - sum) {					//is there a superposition of this block?
				for (; i < limit; i++) {	//O(B-L+S) TODO
					if (line->cells[i]->state == STATE_BLNK) {
						return IMPOSSIBLE;								//impossible
					} else
					if (line->cells[i]->state == STATE_UNKN) {
						line->cells[i]->state = STATE_FULL;
						ret++;
					}
				}
				i += length - sum;										//the next (length - sum) cells are unaffected
			} else {
				i += line->block[n].length;								//skip blocksize cells!
			}
			
			i++;														//1 more cell, for the mandatory space
			n++;														//next block
		}
		return ret;
	}	
	return 0;
}
#undef IMPOSSIBLE




Line* CreateSubLine(Line* line, int start, int end) {
	Line* subline = (Line*) malloc((end-start)*sizeof(Cell*));
	
	return subline;
}

int GetNumberOfPossibleBlocksForCell(Cell* cell) {
	//implementation depends on what we decide on the structures
	return 0;
}


int solveline(Puzzle* puzzle, Line* line, Stack* stack, int coord) {
	int ret;

/*	int i, num;*/
	ret = 0;/*
	for (i = 0; i < puzzle->length[coord]; i++) {
		num = GetNumberOfPossibleBlocksForCell(line->cells[i]);
		if (num == 1) {
			if (/*num of valid cells around current cell (inclusive) < 2*block length*//* 1) {
				Line* subline = CreateSubLine(line, start, end);	//line we wish to get a subline from, starting index to be first position, and index we want as the last position of the subline (both inclusive)
				ret = stackline(subline);
			}
		} else
		if (num == 0) {
			Line->cells[i]->state = STATE_BLNK;
			ret++;
		}
	}
*/
	return ret;
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
	while (!(IsStackEmpty(stack[ROW]) && IsStackEmpty(stack[COL])) && unsolvedCellCount > 0) {
		if (!IsStackEmpty(stack[ROW])) {
			unsolvedCellCount -= solveline(puzzle, Pop(stack[ROW]), stack[COL], ROW);
		}
		if (!IsStackEmpty(stack[COL])) {
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



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * presolve: O(TODO)		- Calls stackline on every row and on every column once to solve 	*
  *							the easiest cells, to pave the way for the fuller linesolver.				*
  *																										*
  * @param Puzzle* :																					*
  *	@return int :			number of unsolved cells left in the puzzle									*
  * 																									*
  * @verbose : 				  Solves cells implicitly based on each line's native characteristics, ie	*
  * 						block numbers and block sizes. Basically fetches all of the basic "easy"	*
  *							information for a puzzle before the program is allowed to go into the more	*
  *							complete algorithm.	For more information check the stackline function.		*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int presolve(Puzzle* puzzle) {	//O(Lr*max((O(L), O(N*B), O(N*(B-L+S)))) + O(Lc*max((O(L), O(N*B), O(N*(B-L+S)))) TODO
	int i, unsolvedCellCount = puzzle->length[ROW] * puzzle->length[COL];
	for (i = 0; i < puzzle->length[ROW]; i++) unsolvedCellCount -= stackline(&puzzle->line[ROW][i], ROW);	//O(Lr*max((O(L), O(N*B), O(N*(B-L+S)))) TODO
	for (i = 0; i < puzzle->length[COL]; i++) unsolvedCellCount -= stackline(&puzzle->line[COL][i], ROW);	//O(Lc*max((O(L), O(N*B), O(N*(B-L+S)))) TODO
	
	return unsolvedCellCount;
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * FreeStacks: O(1)		- Just a macro function to clean up main. Frees memory allocated to stacks. *
  *																										*
  * @param Stack** :		stack array to destroy														*
  *	@noreturn																							*
  * 																									*
  * @verbose : 				nothing to be verbose about													*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FreeStacks(Stack** stack) {
	ClearStack(stack[ROW]);
	ClearStack(stack[COL]);
	free(stack[ROW]);
	free(stack[COL]);
	free(stack);
}

//possibly use a single stack, defining the axis that's currently being used in a static int in a short function, instead of repeatedly passing it along
//is it really necessary to have 2 stacks? they store line pointers, so it shouldn't matter if it's a row or a column, except for the issue with line length
int main (int n, char** args) {
	if (n < 2) errorout(ERROR_ARGS, "No file name was given.");	
	
	Puzzle* puzzle = getPuzzle(args[1]);	//O(TODO)
	int i;
	
	Stack** stack = (Stack**) malloc(AXES*sizeof(Stack*));	
	stack[ROW] = CreateStack();
	for (i = 0; i < puzzle->length[ROW]; i++)	Push(stack[ROW], (void*) &puzzle->line[ROW][i]);	//O(Lr)
	stack[COL] = CreateStack();
	for (i = 0; i < puzzle->length[COL]; i++)	Push(stack[COL], (void*) &puzzle->line[COL][i]);	//O(Lc)

	int unsolvedCellCount;
	if ((unsolvedCellCount = presolve(puzzle)) != 0)	//O(TODO)
		solve(puzzle, stack, unsolvedCellCount);		//O(TODO)

	FreePuzzle(puzzle);	//O(TODO)
	FreeStacks(stack);
	ExportSolution(NULL);
	
	return 0;	
}

//		int opCoord = (coord == ROW ? COL : ROW);

//maybe track amount of unsolved cells per line, and if a line is fully solved, don't push it onto the stack!
